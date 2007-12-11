/*
 *    Copyright 2006 Intel Corporation
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#  include <dtn-config.h>
#endif

#include <oasys/util/OptParser.h>
#include <oasys/util/Time.h>

#include "CLConnection.h"
#include "bundling/BundleDaemon.h"
#include "bundling/BundlePayload.h"
#include "contacts/ContactManager.h"

namespace dtn {

//----------------------------------------------------------------------
CLConnection::CLConnection(const char*       classname,
                           const char*       logpath,
                           ConnectionConvergenceLayer* cl,
                           LinkParams*       params,
                           bool              active_connector)
    : Thread(classname),
      Logger(classname, logpath),
      contact_(classname),
      contact_up_(false),
      cmdqueue_(logpath, &inflight_lock_, false),
      cl_(cl),
      params_(params),
      active_connector_(active_connector),
      num_pollfds_(0),
      poll_timeout_(-1),
      contact_broken_(false)
{
    sendbuf_.reserve(params_->sendbuf_len_);
    recvbuf_.reserve(params_->recvbuf_len_);
}

//----------------------------------------------------------------------
CLConnection::~CLConnection()
{
}

//----------------------------------------------------------------------
void
CLConnection::run()
{
    struct pollfd* cmdqueue_poll;

    initialize_pollfds();
    if (contact_broken_) {
        log_debug("contact_broken set during initialization");
        return;
    }

    cmdqueue_poll         = &pollfds_[num_pollfds_];
    cmdqueue_poll->fd     = cmdqueue_.read_fd();
    cmdqueue_poll->events = POLLIN;

    // based on the parameter passed to the constructor, we either
    // initiate a connection or accept one, then move on to the main
    // run() loop. it is the responsibility of the underlying CL to
    // make sure that a contact_ structure is found / created
    if (active_connector_) {
        connect();
    } else {
        accept();
    }

    oasys::Time next_write(0,0);
    
    while (true) {
        if (contact_broken_) {
            log_debug("contact_broken set, exiting main loop");
            return;
        }

        // check the comand queue coming in from the bundle daemon
        // if any arrive, we continue to the top of the loop to check
        // contact_broken and then process any other commands before
        // checking for data to/from the remote side
        if (cmdqueue_.size() != 0) {
            process_command();
            continue;
        }

        oasys::Time now = oasys::Time::now();
        
        int timeout;
        if (params_->test_write_delay_ == 0)
        {
            // send any data there is to send. if something was sent
            // out and there's still more to go, we'll call poll() with a
            // zero timeout so we can read any data there is to
            // consume, then return to send another chunk.
            bool more_to_send = send_pending_data();
            timeout = more_to_send ? 0 : poll_timeout_;
        }
        else
        {
            // to implement the test_write_delay we need to track the
            // time to call write again
            if (now >= next_write) {
                bool more_to_send = send_pending_data();
                if (more_to_send) {
                    next_write = now;
                    next_write.add_milliseconds(params_->test_write_delay_);
                } else {
                    next_write.sec_  = 0;
                    next_write.usec_ = 0;
                }
            }

            // if next_write is non-zero, then there's more to send.
            if (next_write.sec_ != 0) {
                timeout = std::min((u_int32_t)poll_timeout_,
                                   (next_write - now).in_milliseconds());
            } else {
                timeout = poll_timeout_;
            }

            log_debug("timeout is %u: next_write %u.%u (%u ms from now), poll_timeout %d",
                      timeout, next_write.sec_, next_write.usec_, 
                      next_write.sec_ == 0 ? 0 : (next_write - now).in_milliseconds(), poll_timeout_);
            
        }
        
        // check again here for contact broken since we don't want to
        // poll if the socket's been closed
        if (contact_broken_) {
            log_debug("contact_broken set, exiting main loop");
            return;
        }
        
        // now we poll() to wait for a new command (indicated by the
        // notifier on the command queue), data arriving from the
        // remote side, or write-readiness on the socket indicating
        // that we can send more data.
        for (int i = 0; i < num_pollfds_ + 1; ++i) {
            pollfds_[i].revents = 0;
        }

        log_debug("calling poll on %d fds with timeout %d",
                  num_pollfds_ + 1, timeout);
                                                 
        int cc = oasys::IO::poll_multiple(pollfds_, num_pollfds_ + 1,
                                          timeout, NULL, logpath_);
                                          
        // check again here for contact broken since we don't want to
        // act on the poll result if the contact is broken
        if (contact_broken_) {
            log_debug("contact_broken set, exiting main loop");
            return;
        }
        
        if (cc == oasys::IOTIMEOUT)
        {
            handle_poll_timeout();
        }
        else if (cc > 0)
        {
            if (cc == 1 && cmdqueue_poll->revents != 0) {
                continue; // activity on the command queue only
            }
            handle_poll_activity();
        }
        else
        {
            log_err("unexpected return from poll_multiple: %d", cc);
            break_contact(ContactEvent::BROKEN);
            return;
        }
    }
}

//----------------------------------------------------------------------
void
CLConnection::bundles_queued()
{
    ASSERT(contact_->link() != NULL);
    ASSERT(contact_->link()->cl_info() != NULL);

    /*
     * Called by the main BundleDaemon thread... push a new CLMsg onto
     * the queue and potentially set the BUSY state on the link. Note
     * that it's important to update num_pending before pushing the
     * message onto the queue since the latter might trigger a context
     * switch.
     */
    LinkParams* params = dynamic_cast<LinkParams*>(contact_->link()->cl_info());
    ASSERT(params != NULL);
    
    cmdqueue_.push_back(
        CLConnection::CLMsg(CLConnection::CLMSG_BUNDLES_QUEUED));
}

//----------------------------------------------------------------------
void
CLConnection::cancel_bundle(Bundle* bundle)
{
    ASSERT(contact_->link() != NULL);
    ASSERT(contact_->link()->cl_info() != NULL);
    cmdqueue_.push_back(
        CLConnection::CLMsg(CLConnection::CLMSG_CANCEL_BUNDLE, bundle));
}

//----------------------------------------------------------------------
void
CLConnection::process_command()
{
    CLMsg msg;
    bool ok = cmdqueue_.try_pop(&msg);
    ASSERT(ok); // shouldn't be called if the queue is empty
    
    switch(msg.type_) {
    case CLMSG_BUNDLES_QUEUED:
        log_debug("processing CLMSG_BUNDLES_QUEUED");
        handle_bundles_queued();
        break;
        
    case CLMSG_CANCEL_BUNDLE:
        log_debug("processing CLMSG_CANCEL_BUNDLE");
        handle_cancel_bundle(msg.bundle_.object());
        break;
        
    case CLMSG_BREAK_CONTACT:
        log_debug("processing CLMSG_BREAK_CONTACT");
        break_contact(ContactEvent::USER);
        break;
    default:
        PANIC("invalid CLMsg typecode %d", msg.type_);
    }
}

//----------------------------------------------------------------------
void
CLConnection::contact_up()
{
    log_debug("contact_up");
    ASSERT(contact_ != NULL);
    
    ASSERT(!contact_up_);
    contact_up_ = true;
    
    BundleDaemon::post(new ContactUpEvent(contact_));
}

//----------------------------------------------------------------------
void
CLConnection::break_contact(ContactEvent::reason_t reason)
{
    contact_broken_ = true;
        
    log_debug("break_contact: %s", ContactEvent::reason_to_str(reason));

    if (reason != ContactEvent::BROKEN) {
        disconnect();
    }

    // if the connection isn't being closed by the user, we need to
    // notify the daemon that either the contact ended or the link
    // became unavailable before a contact began.
    //
    // we need to check that there is in fact a contact, since a
    // connection may be accepted and then break before establishing a
    // contact
    if ((reason != ContactEvent::USER) && (contact_ != NULL)) {
        BundleDaemon::post(
            new LinkStateChangeRequest(contact_->link(),
                                       Link::CLOSED,
                                       reason));
    }
}

//----------------------------------------------------------------------
void
CLConnection::close_contact()
{
    LinkRef link = contact_->link();
    ASSERT(link != NULL);
    ASSERT(link->cl_info() != NULL);

    LinkParams* params = dynamic_cast<LinkParams*>(link->cl_info());
    ASSERT(params != NULL);
    
    oasys::ScopeLock l(inflight_lock(), "CLConnection::close_contact");

    // drain the inflight queue, posting transmitted or transmit
    // failed events
    while (! inflight_.empty()) {
        InFlightBundle* inflight = inflight_.front();
        u_int32_t sent_bytes  = inflight->sent_data_.num_contiguous();
        u_int32_t acked_bytes = inflight->ack_data_.num_contiguous();

        if ((! params->reactive_frag_enabled_) ||
            (sent_bytes == 0) ||
            (link->is_reliable() && acked_bytes == 0))
        {
            /*log_debug("posting transmission failed event "
                      "(reactive fragmentation %s, %s link, acked_bytes %u)",
                      params->reactive_frag_enabled_ ? "enabled" : "disabled",
                      link->is_reliable() ? "reliable" : "unreliable",
                      acked_bytes);
            
            BundleDaemon::post(
                new BundleTransmitFailedEvent(inflight->bundle_.object(),
                                              contact_, link));*/
            
        } else {
            BundleDaemon::post(
                new BundleTransmittedEvent(inflight->bundle_.object(),
                                           contact_, link,
                                           sent_bytes, acked_bytes));
        }

        inflight_.pop_front();
        delete inflight;
    }

    // check the tail of the incoming queue to see if there's a
    // partially-received bundle that we need to post a received event
    // for (if reactive fragmentation is enabled)
    if (! incoming_.empty()) {
        IncomingBundle* incoming = incoming_.back();
        if(!incoming->rcvd_data_.empty())
        {  
            size_t rcvd_len = incoming->rcvd_data_.last() + 1;

            size_t header_block_length =
                BundleProtocol::payload_offset(&incoming->bundle_->recv_blocks_);
        
            if ((incoming->total_length_ == 0) && 
                params->reactive_frag_enabled_ &&
                (rcvd_len > header_block_length))
            {
                log_debug("partial arrival of bundle: "
                          "got %zu bytes [hdr %zu payload %zu]",
                          rcvd_len, header_block_length,
                          incoming->bundle_->payload_.length());
             
                BundleDaemon::post(
                    new BundleReceivedEvent(incoming->bundle_.object(),
                                            EVENTSRC_PEER, rcvd_len,
                                            contact_.object()));
            }
        }
    }

    // now drain the incoming queue
    while (!incoming_.empty()) {
        IncomingBundle* incoming = incoming_.back();
        incoming_.pop_back();
        delete incoming;
    }
}

//----------------------------------------------------------------------
bool
CLConnection::find_contact(const EndpointID& peer_eid)
{
    if (contact_ != NULL) {
        log_debug("CLConnection::find_contact: contact already exists");
        return true;
    }
        
    /*
     * Now we may need to find or create an appropriate opportunistic
     * link for the connection.
     *
     * First, we check if there's an idle (i.e. UNAVAILABLE) link to
     * the remote eid. We explicitly ignore the nexthop address, since
     * that can change (due to things like TCP/UDP port number
     * assignment), but we pass in the remote eid to match for a link.
     *
     * If we can't find one, then we create a new opportunistic link
     * for the connection.
     */
    ASSERT(nexthop_ != ""); // the derived class must have set the
                            // nexthop in the constructor

    ContactManager* cm = BundleDaemon::instance()->contactmgr();
    oasys::ScopeLock l(cm->lock(), "CLConnection::find_contact");

    bool new_link = false;
    LinkRef link = cm->find_link_to(cl_, "", peer_eid,
                                    Link::OPPORTUNISTIC,
                                    Link::AVAILABLE | Link::UNAVAILABLE);

    if (link == NULL || (link != NULL && link->contact() != NULL)) {
        if (link != NULL) {
            log_warn("CLConnection::find_contact: "
                     "in-use opportunistic link *%p", link.object());
        }

        link = cm->new_opportunistic_link(cl_, nexthop_.c_str(), peer_eid);
        if (link == NULL) {
            log_debug("CLConnection::find_contact: "
                      "failed to create opportunistic link");
            return false;
        }

        new_link = true;
        log_debug("CLConnection::find_contact: "
                  "created new opportunistic link *%p", link.object());
    }

    ASSERT(link != NULL);
    oasys::ScopeLock link_lock(link->lock(), "CLConnection::find_contact");

    // XXX/demmer remove check for no contact
    if (!new_link) {
        ASSERT(link->contact() == NULL);
        link->set_nexthop(nexthop_);
        log_debug("CLConnection::find_contact: "
                  "found idle opportunistic link *%p", link.object());
    }

    // The link should not be marked for deletion because the
    // ContactManager is locked.
    ASSERT(!link->isdeleted());

    ASSERT(link->cl_info() != NULL);
    ASSERT(!link->isopen());

    contact_ = new Contact(link);
    contact_->set_cl_info(this);
    link->set_contact(contact_.object());

    /*
     * Now that the connection is established, we swing the
     * params_ pointer to those of the link, since there's a
     * chance they've been modified by the user in the past.
     */
    LinkParams* lparams = dynamic_cast<LinkParams*>(link->cl_info());
    ASSERT(lparams != NULL);
    params_ = lparams;

    return true;
}


} // namespace dtn
