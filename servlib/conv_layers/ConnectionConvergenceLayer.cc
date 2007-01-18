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


#include <oasys/util/OptParser.h>

#include "ConnectionConvergenceLayer.h"
#include "CLConnection.h"
#include "bundling/BundleDaemon.h"

namespace dtn {

//----------------------------------------------------------------------
ConnectionConvergenceLayer::LinkParams::LinkParams(bool init_defaults)
    : busy_queue_depth_(10),
      reactive_frag_enabled_(true),
      sendbuf_len_(32768),
      recvbuf_len_(32768),
      data_timeout_(30000), // msec
      test_read_delay_(0),
      test_write_delay_(0),
      test_recv_delay_(0)
{
    (void)init_defaults;
}

//----------------------------------------------------------------------
ConnectionConvergenceLayer::ConnectionConvergenceLayer(const char* classname,
                                                       const char* cl_name)
    : ConvergenceLayer(classname, cl_name)
{
}

//----------------------------------------------------------------------
bool
ConnectionConvergenceLayer::parse_link_params(LinkParams* params,
                                              int argc, const char** argv,
                                              const char** invalidp)
{
    oasys::OptParser p;
    
    p.addopt(new oasys::UIntOpt("busy_queue_depth",
                                &params->busy_queue_depth_));    
    p.addopt(new oasys::BoolOpt("reactive_frag_enabled",
                                &params->reactive_frag_enabled_));
    p.addopt(new oasys::UIntOpt("sendbuf_len", &params->sendbuf_len_));
    p.addopt(new oasys::UIntOpt("recvbuf_len", &params->recvbuf_len_));
    p.addopt(new oasys::UIntOpt("data_timeout", &params->data_timeout_));
    
    p.addopt(new oasys::UIntOpt("test_read_delay",
                                &params->test_read_delay_));
    p.addopt(new oasys::UIntOpt("test_write_delay",
                                &params->test_write_delay_));
    p.addopt(new oasys::UIntOpt("test_recv_delay",
                                &params->test_recv_delay_));
    
    if (! p.parse(argc, argv, invalidp)) {
        return false;
    }
    
    if (params->sendbuf_len_ == 0) {
        *invalidp = "sendbuf_len must not be zero";
        return false;
    }

    if (params->recvbuf_len_ == 0) {
        *invalidp = "recvbuf_len must not be zero";
        return false;
    }
    
    return true;
}

//----------------------------------------------------------------------
void
ConnectionConvergenceLayer::dump_link(const LinkRef& link,
                                      oasys::StringBuffer* buf)
{
    LinkParams* params = dynamic_cast<LinkParams*>(link->cl_info());
    ASSERT(params != NULL);
    
    buf->appendf("busy_queue_depth: %u\n", params->busy_queue_depth_);
    buf->appendf("reactive_frag_enabled: %u\n", params->reactive_frag_enabled_);
    buf->appendf("sendbuf_len: %u\n", params->sendbuf_len_);
    buf->appendf("recvbuf_len: %u\n", params->recvbuf_len_);
    buf->appendf("data_timeout: %u\n", params->data_timeout_);
    buf->appendf("test_read_delay: %u\n", params->test_read_delay_);
    buf->appendf("test_write_delay: %u\n", params->test_write_delay_);
    buf->appendf("test_recv_delay: %u\n",params->test_recv_delay_);
}

//----------------------------------------------------------------------
bool
ConnectionConvergenceLayer::init_link(const LinkRef& link,
                                      int argc, const char* argv[])
{
    log_debug("adding %s link %s", link->type_str(), link->nexthop());

    // Create a new parameters structure, parse the options, and store
    // them in the link's cl info slot.
    LinkParams* params = new_link_params();

    // Try to parse the link's next hop, but continue on even if the
    // parse fails since the hostname may not be resolvable when we
    // initialize the link. Each subclass is responsible for
    // re-checking when opening the link.
    parse_nexthop(link, params);
    
    const char* invalid;
    if (! parse_link_params(params, argc, argv, &invalid)) {
        log_err("error parsing link options: invalid option '%s'", invalid);
        delete params;
        return false;
    }

    if (! finish_init_link(link, params)) {
        log_err("error in finish_init_link");
        delete params;
        return false;
    }

    link->set_cl_info(params);

    return true;
}

//----------------------------------------------------------------------
bool
ConnectionConvergenceLayer::finish_init_link(const LinkRef& link,
                                             LinkParams* params)
{
    (void)link;
    (void)params;
    return true;
}

//----------------------------------------------------------------------
bool
ConnectionConvergenceLayer::reconfigure_link(const LinkRef& link,
                                             int argc, const char* argv[])
{
    LinkParams* params = dynamic_cast<LinkParams*>(link->cl_info());
    ASSERT(params != NULL);
    
    const char* invalid;
    if (! parse_link_params(params, argc, argv, &invalid)) {
        log_err("reconfigure_link: invalid parameter %s", invalid);
        return false;
    }

    if (link->isopen()) {
        LinkParams* params = dynamic_cast<LinkParams*>(link->cl_info());
        ASSERT(params != NULL);
        
        CLConnection* conn = dynamic_cast<CLConnection*>(link->contact()->cl_info());
        ASSERT(conn != NULL);
        
        if ((params->sendbuf_len_ != conn->sendbuf_.size()) &&
            (params->sendbuf_len_ >= conn->sendbuf_.fullbytes()))
        {
            log_info("resizing link *%p send buffer from %zu -> %u",
                     link.object(), conn->sendbuf_.size(),
                     params->sendbuf_len_);
            conn->sendbuf_.set_size(params->sendbuf_len_);
        }

        if ((params->recvbuf_len_ != conn->recvbuf_.size()) &&
            (params->recvbuf_len_ >= conn->recvbuf_.fullbytes()))
        {
            log_info("resizing link *%p recv buffer from %zu -> %u",
                     link.object(), conn->recvbuf_.size(),
                     params->recvbuf_len_);
            conn->recvbuf_.set_size(params->recvbuf_len_);
        }
    }

    return true;
}

//----------------------------------------------------------------------
bool
ConnectionConvergenceLayer::open_contact(const ContactRef& contact)
{
    LinkRef link = contact->link();
    log_debug("opening contact on link *%p", link.object());
    
    LinkParams* params = dynamic_cast<LinkParams*>(link->cl_info());
    ASSERT(params != NULL);
    
    // create a new connection for the contact, set up to use the
    // link's configured parameters
    CLConnection* conn = new_connection(params);
    conn->set_contact(contact);
    contact->set_cl_info(conn);
    conn->start();

    return true;
}

//----------------------------------------------------------------------
bool
ConnectionConvergenceLayer::close_contact(const ContactRef& contact)
{
    log_info("close_contact *%p", contact.object());

    CLConnection* conn = dynamic_cast<CLConnection*>(contact->cl_info());
    ASSERT(conn != NULL);

    // if the connection isn't already broken, then we need to tell it
    // to do so
    if (! conn->contact_broken_) {
        conn->cmdqueue_.push_back(
            CLConnection::CLMsg(CLConnection::CLMSG_BREAK_CONTACT));
    }
    
    while (!conn->is_stopped()) {
        log_debug("waiting for connection thread to stop...");
        usleep(100000);
        oasys::Thread::yield();
    }

    conn->close_contact();
    delete conn;

    contact->set_cl_info(NULL);

    return true;
}

//----------------------------------------------------------------------
void
ConnectionConvergenceLayer::send_bundle(const ContactRef& contact,
                                        Bundle* bundle)
{
    log_debug("send_bundle *%p to *%p", bundle, contact.object());

    CLConnection* conn = dynamic_cast<CLConnection*>(contact->cl_info());
    ASSERT(conn != NULL);

    conn->queue_bundle(bundle);
}

} // namespace dtn
