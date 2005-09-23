/*
 * IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. By
 * downloading, copying, installing or using the software you agree to
 * this license. If you do not agree to this license, do not download,
 * install, copy or use the software.
 * 
 * Intel Open Source License 
 * 
 * Copyright (c) 2004 Intel Corporation. All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 
 *   Neither the name of the Intel Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <sys/poll.h>

#include <oasys/io/NetUtils.h>
#include <oasys/thread/Timer.h>
#include <oasys/util/OptParser.h>
#include <oasys/util/StringBuffer.h>
#include <oasys/util/URL.h>

#include "UDPConvergenceLayer.h"
#include "bundling/Bundle.h"
#include "bundling/BundleEvent.h"
#include "bundling/BundleDaemon.h"
#include "bundling/BundleList.h"
#include "bundling/BundleProtocol.h"

namespace dtn {

struct UDPConvergenceLayer::Params UDPConvergenceLayer::defaults_;

/******************************************************************************
 *
 * UDPConvergenceLayer
 *
 *****************************************************************************/

UDPConvergenceLayer::UDPConvergenceLayer()
    : IPConvergenceLayer("/cl/udp")
{
    defaults_.local_addr_		= INADDR_ANY;
    defaults_.local_port_		= 5000;
    defaults_.remote_addr_		= INADDR_NONE;
    defaults_.remote_port_		= 0;
}

/**
 * Parse variable args into a parameter structure.
 */
bool
UDPConvergenceLayer::parse_params(Params* params,
                                  int argc, const char** argv,
                                  const char** invalidp)
{
    oasys::OptParser p;

    p.addopt(new oasys::InAddrOpt("local_addr", &params->local_addr_));
    p.addopt(new oasys::UInt16Opt("local_port", &params->local_port_));
    p.addopt(new oasys::InAddrOpt("remote_addr", &params->remote_addr_));
    p.addopt(new oasys::UInt16Opt("remote_port", &params->remote_port_));

    if (! p.parse(argc, argv, invalidp)) {
        return false;
    }

    return true;
};

/**
 * Bring up a new interface.
 */
bool
UDPConvergenceLayer::interface_up(Interface* iface,
                                  int argc, const char* argv[])
{
    log_debug("adding interface %s", iface->name().c_str());
    
    // parse options (including overrides for the local_addr and
    // local_port settings from the defaults)
    Params params = UDPConvergenceLayer::defaults_;
    const char* invalid;
    if (!parse_params(&params, argc, argv, &invalid)) {
        log_err("error parsing interface options: invalid option '%s'",
                invalid);
        return false;
    }

    // check that the local interface / port are valid
    if (params.local_addr_ == INADDR_NONE) {
        log_err("invalid local address setting of 0");
        return false;
    }

    if (params.local_port_ == 0) {
        log_err("invalid local port setting of 0");
        return false;
    }
    
    // create a new server socket for the requested interface
    Receiver* receiver = new Receiver(&params);
    receiver->logpathf("%s/iface/%s", logpath_, iface->name().c_str());
    
    if (receiver->bind(params.local_addr_, params.local_port_) != 0) {
        return false; // error log already emitted
    }
    
    // check if the user specified a remote addr/port to connect to
    if (params.remote_addr_ != INADDR_NONE) {
        if (receiver->connect(params.remote_addr_, params.remote_port_) != 0) {
            return false; // error log already emitted
        }
    }
    
    // start the thread which automatically listens for data
    receiver->start();
    
    // store the new listener object in the cl specific portion of the
    // interface
    iface->set_cl_info(receiver);
    
    return true;
}

/**
 * Bring down the interface.
 */
bool
UDPConvergenceLayer::interface_down(Interface* iface)
{
    // grab the listener object, set a flag for the thread to stop and
    // then close the socket out from under it, which should cause the
    // thread to break out of the blocking call to accept() and
    // terminate itself
    Receiver* receiver = (Receiver*)iface->cl_info();
    receiver->set_should_stop();
    receiver->interrupt_from_io();
    
    while (! receiver->is_stopped()) {
        oasys::Thread::yield();
    }

    delete receiver;
    return true;
}

/**
 * Dump out CL specific interface information.
 */
void
UDPConvergenceLayer::dump_interface(Interface* iface,
                                    oasys::StringBuffer* buf)
{
    Params* params = &((Receiver*)iface->cl_info())->params_;
    
    buf->appendf("\tlocal_addr: %s local_port: %d\n",
                 intoa(params->local_addr_), params->local_port_);
    
    if (params->remote_addr_ != INADDR_NONE) {
        buf->appendf("\tconnected remote_addr: %s remote_port: %d\n",
                     intoa(params->remote_addr_), params->remote_port_);
    } else {
        buf->appendf("\tnot connected\n");
    }
}

/**
 * Create any CL-specific components of the Link.
 *
 * This parses and validates the parameters and stores them in the
 * CLInfo slot in the Link class.
 */
bool
UDPConvergenceLayer::init_link(Link* link, int argc, const char* argv[])
{
    in_addr_t addr;
    u_int16_t port = 0;
    
    log_debug("adding %s link %s", link->type_str(), link->nexthop());

    // validate the link next hop address
    if (! IPConvergenceLayer::parse_nexthop(link->nexthop(), &addr, &port)) {
        log_err("invalid next hop address '%s'", link->nexthop());
        return false;
    }

    // make sure it's really a valid address
    if (addr == INADDR_ANY || addr == INADDR_NONE) {
        log_err("invalid host in next hop address '%s'", link->nexthop());
        return false;
    }
    
    // make sure the port was specified
    if (port == 0) {
        log_err("port not specified in next hop address '%s'",
                link->nexthop());
        return false;
    }

    // Create a new parameters structure, parse the options, and store
    // them in the link's cl info slot
    Params* params = new Params(defaults_);
    params->local_addr_ = INADDR_NONE;
    params->local_port_ = 0;

    const char* invalid;
    if (! parse_params(params, argc, argv, &invalid)) {
        log_err("error parsing link options: invalid option '%s'", invalid);
        delete params;
        return false;
    }

    link->set_cl_info(params);
    return true;
}

/**
 * Dump out CL specific link information.
 */
void
UDPConvergenceLayer::dump_link(Link* link, oasys::StringBuffer* buf)
{
    Params* params = (Params*)link->cl_info();
    
    buf->appendf("\tlocal_addr: %s local_port: %d\n",
                 intoa(params->local_addr_), params->local_port_);

    buf->appendf("\tremote_addr: %s remote_port: %d\n",
                 intoa(params->remote_addr_), params->remote_port_);
}

bool
UDPConvergenceLayer::open_contact(Contact* contact)
{
    in_addr_t addr;
    u_int16_t port;
    
    log_debug("opening contact *%p", contact);

    Link* link = contact->link();
    
    // parse out the address / port from the nexthop address. note
    // that these should have been validated in init_link() above, so
    // we ASSERT as such
    bool valid = parse_nexthop(link->nexthop(), &addr, &port);
    ASSERT(valid == true);
    ASSERT(addr != INADDR_NONE && addr != INADDR_ANY);
    ASSERT(port != 0);
    
    Params* params = (Params*)link->cl_info();
    
    // create a new connected socket for the contact
    Sender* sender = new Sender(contact);
    sender->logpathf("/cl/udp/conn/%s:%d", intoa(addr), port);
    sender->set_logfd(false);

    if (params->local_addr_ != INADDR_NONE || params->local_port_ != 0)
    {
        if (sender->bind(params->local_addr_, params->local_port_) != 0) {
            log_err("error binding to %s:%d: %s",
                    intoa(params->local_addr_), params->local_port_,
                    strerror(errno));
            delete sender;
            return false;
        }
    }
    
    if (sender->connect(addr, port) != 0) {
        log_err("error issuing udp connect to %s:%d: %s",
                intoa(addr), port, strerror(errno));
        delete sender;
        return false;
    }
    
    contact->set_cl_info(sender);

    BundleDaemon::post(new ContactUpEvent(contact));
    
    return true;
}

bool
UDPConvergenceLayer::close_contact(Contact* contact)
{
    Sender* sender = (Sender*)contact->cl_info();
    
    log_info("close_contact *%p", contact);

    if (sender) {
        delete sender;
        contact->set_cl_info(NULL);
    }
    
    return true;
}

/**
 * Send bundles queued for the contact. We only expect there to be
 * one bundle queued at any time since this is called immediately
 * when the bundle is queued on the contact.
 */
void
UDPConvergenceLayer::send_bundle(Contact* contact, Bundle* bundle)
{
    Sender* sender = (Sender*)contact->cl_info();
    if (!sender) {
        log_crit("send_bundles called on contact *%p with no Sender!!",
                 contact);
        return;
    }
    ASSERT(contact == sender->contact_);

    sender->send_bundle(bundle); // consumes bundle reference
}

/******************************************************************************
 *
 * UDPConvergenceLayer::Receiver
 *
 *****************************************************************************/
UDPConvergenceLayer::Receiver::Receiver(UDPConvergenceLayer::Params* params)
    : IOHandlerBase(new oasys::Notifier()),
      UDPClient("/cl/udp/receiver"),
      Thread("UDPConvergenceLayer::Receiver")
{
    logfd_  = false;
    params_ = *params;
}

void
UDPConvergenceLayer::Receiver::process_data(u_char* bp, size_t len)
{
    Bundle* bundle = NULL;       
    int header_len;
    
    // parse the headers into a new bundle. this sets the payload_len
    // appropriately in the new bundle and returns the number of bytes
    // consumed for the bundle headers
    bundle = new Bundle();
    header_len = BundleProtocol::parse_headers(bundle, bp, len);

    if (header_len < 0) {
        log_err("process_data: invalid or too short bundle header");
        delete bundle;
        return;
    }
    
    size_t payload_len = bundle->payload_.length();

    if (len != header_len + payload_len) {
        log_err("process_data: error in bundle lengths: "
                "bundle_length %u, header_length %u, payload_length %u",
                (u_int)len, (u_int)header_len, (u_int)payload_len);
        delete bundle;
        return;
    }

    // store the payload and notify the daemon
    bundle->payload_.set_data(bp + header_len, payload_len);
    
    log_debug("process_data: new bundle id %d arrival, payload length %u",
	      bundle->bundleid_, (u_int)bundle->payload_.length());
    
    BundleDaemon::post(
        new BundleReceivedEvent(bundle, EVENTSRC_PEER, payload_len));
}

void
UDPConvergenceLayer::Receiver::run()
{
    int ret;
    in_addr_t addr;
    u_int16_t port;
    u_char buf[MAX_UDP_PACKET];

    while (1) {
        if (should_stop())
            break;
        
        ret = recvfrom((char*)buf, MAX_UDP_PACKET, 0, &addr, &port);
	if (ret <= 0) {	  
            if (errno == EINTR) {
                continue;
            }
            log_err("error in recvfrom(): %d %s",
                    errno, strerror(errno));
            close();
            break;
        }
        
	log_debug("got %d byte packet from %s:%d",
                  ret, intoa(addr), port);	         
	process_data(buf, ret);
    }
}

/******************************************************************************
 *
 * UDPConvergenceLayer::Sender
 *
 *****************************************************************************/

/**
 * Constructor for the active connection side of a connection.
 */
UDPConvergenceLayer::Sender::Sender(Contact* contact)
    : contact_(contact)
{
}
        
/* 
 * Send one bundle.
 */
bool 
UDPConvergenceLayer::Sender::send_bundle(Bundle* bundle)
{
    bool ok;
    int header_len;
    size_t payload_len = bundle->payload_.length();

    // stuff in the bundle headers
    header_len = BundleProtocol::format_headers(bundle, buf_, sizeof(buf_));
    if (header_len < 0) {
        log_err("send_bundle: bundle header too big for buffer (len %u)",
                (u_int)sizeof(buf_));
        return false;
    }

    // check that the payload isn't too big
    // XXX/demmer maybe we need to fragment here? or return an MTU for
    // the higher layer
    if (payload_len > (sizeof(buf_) - header_len)) {
        log_err("send_bundle: bundle payload + headers (length %u) too big",
                (u_int)(header_len + payload_len));
        return false;
    }

    // read the payload data into the buffer
    bundle->payload_.read_data(0, payload_len, &buf_[header_len],
                               BundlePayload::FORCE_COPY);

    // write it out the socket and make sure we wrote it all
    int cc = write((char*)buf_, header_len + payload_len);
    if (cc == (int)(header_len + payload_len)) {
        log_err("send_bundle: sent bundle length %d", cc);
        BundleDaemon::post(
            new BundleTransmittedEvent(bundle, contact_->link(),
                                       bundle->payload_.length(),
                                       false));
        ok = true;
    } else {
        log_err("send_bundle: error sending bundle (wrote %d/%u): %s",
                cc, (u_int)(header_len + payload_len), strerror(errno));
        ok = false;
    }

    return ok;
}

} // namespace dtn
