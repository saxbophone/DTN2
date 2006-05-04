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

#include "BundleRouter.h"
#include "RouteTable.h"
#include "bundling/Bundle.h"
#include "bundling/BundleActions.h"
#include "bundling/BundleDaemon.h"
#include "bundling/BundleList.h"
#include "contacts/Contact.h"
#include "reg/Registration.h"
#include <stdlib.h>

#include "FloodBundleRouter.h"

namespace dtn {

/**
 * Constructor.
 */
FloodBundleRouter::FloodBundleRouter()
    : TableBasedRouter("FloodBundleRouter", "flood"),
      all_eids_("dtn://*")
{
    log_info("FloodBundleRouter initialized");

//    router_stats_= new RouterStatistics(this);
}


/**
 * Destructor
 */
FloodBundleRouter::~FloodBundleRouter()
{
    NOTIMPLEMENTED;
}

// XXX/demmer implement me


/**
 * Default event handler for new bundle arrivals.
 *
 * Queue the bundle on the pending delivery list, and then
 * searches through the route table to find any matching next
 * contacts, filling in the action list with forwarding decisions.
 */
void
FloodBundleRouter::handle_bundle_received(BundleReceivedEvent* event)
{
    // XXX/demmer fixme
    NOTIMPLEMENTED;
    
//     Bundle* bundle = event->bundleref_.bundle();
//     log_info("FLOOD: bundle_rcv bundle id %d", bundle->bundleid_);
    
//     /*
//      * Check if the bundle isn't complete. If so, do reactive
//      * fragmentation.
//      *
//      * XXX/demmer this should maybe be changed to do the reactive
//      * fragmentation in the forwarder?
//      */
//     if (event->bytes_received_ != bundle->payload_.length()) {
//         log_info("XXX: PARTIAL bundle:%d, making fragment of %d bytes",
//                   bundle->bundleid_, (u_int)event->bytes_received_);
//         BundleDaemon::instance()->fragmentmgr()->
//             convert_to_fragment(bundle, event->bytes_received_);
//     }
        

//     //statistics
//     //bundle has been accepted
//     //router_stats_->add_bundle_hop(bundle);

    

//     Bundle* iter_bundle;
//     BundleList::iterator iter;

//     oasys::ScopeLock lock(pending_bundles_->lock());
    
//     //check if we already have the bundle with us ... 
//     //then dont enqueue it
//     // upon arrival of new contact, send all pending bundles over contact
//     for (iter = pending_bundles_->begin(); 
//          iter != pending_bundles_->end(); ++iter) {
//         iter_bundle = *iter;
//         log_info("pending_bundle:%d size:%d",
//                  iter_bundle->bundleid_, (u_int)iter_bundle->payload_.length());

//         if (iter_bundle->bundleid_ == bundle->bundleid_) {
//             //delete the bundle
//             return;
//         }
//     }
    
//     pending_bundles_->push_back(bundle);
//     if (event->source_ != EVENTSRC_PEER)
//         actions_->store_add(bundle);
    
//     //here we do not need to handle the new bundle immediately
//     //just put it in the pending_bundles_ queue, and it
//     //needs to be used only when a new contact comes up
//     //**might do something different if the bundle is from
//     //  the local node
//     //BundleRouter::handle_bundle_received(event, actions_);

//     fwd_to_matching(bundle, true);
}

void
FloodBundleRouter::handle_bundle_transmitted(BundleTransmittedEvent* event)
{
    Bundle* bundle = event->bundleref_.object();
    
    //only call the fragmentation routine if we send nonzero bytes
    if(event->bytes_sent_ > 0) {
        BundleRouter::handle_bundle_transmitted(event);         
    } else {
        log_info("FLOOD: transmitted ZERO bytes:%d",bundle->bundleid_);
    }
    
    //now we want to ask the contact to send the other queued
    //bundles it has

    PANIC("XXX/demmer need to kick the contact to tell it to send bundleS");
    //Contact * contact = (Contact *)event->consumer_;
    //contact->enqueue_bundle(NULL, FORWARD_UNIQUE);
    
}

/**
 * Default event handler when a new link is created.
 */
void
FloodBundleRouter::handle_link_created(LinkCreatedEvent* event)
{
    
    Link* link = event->link_;
    ASSERT(link != NULL);
    log_info("FLOOD: LINK_CREATED *%p", event->link_);

    RouteEntry* entry = new RouteEntry(all_eids_, link);
    entry->action_ = FORWARD_COPY;
    add_route(entry);

//    route_table_->add_entry(entry);

    //first clear the list with the contact
//    contact->bundle_list()->clear();

    //copy the pending_bundles_ list into a new exchange list
    //exchange_list_ = pending_bundles_->copy();
    //
//    new_next_hop(all_eids_, link, actions_);
}

/**
 * Default event handler when a contact is down
 */
void
FloodBundleRouter::handle_contact_down(ContactDownEvent* event)
{
    const ContactRef& contact = event->contact_;
    log_info("FLOOD: CONTACT_DOWN *%p: removing queued bundles", contact.object());
    
    //XXX not implemented yet - neeed to do
    route_table_->del_entry(all_eids_, contact->link());
}

/**
 * Called whenever a new consumer (i.e. registration or contact)
 * arrives. This should walk the list of unassigned bundles (or
 * maybe all bundles???) to see if the new consumer matches.
 */
void
FloodBundleRouter::new_next_hop(const EndpointIDPattern& dest, Link* next_hop)
{
    log_debug("FLOOD:  new_next_hop");

    Bundle* bundle;
    BundleList::iterator iter;

    oasys::ScopeLock lock(pending_bundles_->lock(), 
                          "FloodBundleRouter::new_next_hop");
    
    // upon arrival of new contact, send all pending bundles over contact
    for (iter = pending_bundles_->begin(); 
         iter != pending_bundles_->end(); ++iter) {
        bundle = *iter;
        actions_->send_bundle(bundle, next_hop, FORWARD_COPY,
                              CustodyTimerSpec::defaults_);
    }
}


int
FloodBundleRouter::fwd_to_matching(Bundle* bundle, bool include_local)
{
    RouteEntry* entry;
    RouteEntryVec matches;
    RouteEntryVec::iterator iter;

    route_table_->get_matching(bundle->dest_, &matches);
    
    int count = 0;
    for (iter = matches.begin(); iter != matches.end(); ++iter) {
        entry = *iter;
        log_info("\tentry: point:%s --> %s [%s]",
                 entry->pattern_.c_str(),
                 entry->next_hop_->nexthop(),
                 bundle_fwd_action_toa(entry->action_));
        
        actions_->send_bundle(bundle, entry->next_hop_, FORWARD_COPY,
                              CustodyTimerSpec::defaults_);
        ++count;
    }

    PANIC("XXX/demmer fixme");

    log_info("FLOOD: local_fwd_to_matching %s: %d matches", bundle->dest_.c_str(), count);
    return count;
}

} // namespace dtn
