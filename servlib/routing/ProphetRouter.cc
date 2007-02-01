/*
 *    Copyright 2006 Baylor University
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

#include "BundleRouter.h"
#include "bundling/Bundle.h"
#include "bundling/BundleActions.h"
#include "bundling/BundleDaemon.h"
#include "bundling/BundleList.h"
#include "contacts/Contact.h"
#include "contacts/ContactManager.h"
#include <oasys/util/StringBuffer.h>
#include <stdlib.h>

#include "ProphetRouter.h"

namespace dtn {

void
prophet_router_shutdown(void*)
{
    BundleDaemon::instance()->router()->shutdown();
}

ProphetParams ProphetRouter::params_;

ProphetRouter::ProphetRouter()
    : BundleRouter("ProphetRouter", "prophet")
{
    log_debug("ProphetRouter constructor");
    // params_ default values are set by ProphetLists.h
}

void
ProphetRouter::shutdown()
{
    oracle_->shutdown();
}

ProphetRouter::~ProphetRouter()
{
    delete oracle_;
}

void
ProphetRouter::initialize()
{
    // Call factory to instantiate ProphetController
    ProphetController::init(&params_,pending_bundles_,actions_);

    // For convenience, grab hold of that instance variable
    oracle_ = ProphetController::instance();

    // Register the global shutdown function
    BundleDaemon::instance()->set_rtr_shutdown(
                prophet_router_shutdown, (void *) 0);

    // all done
    log_info("ProphetRouter initialized");
}

void
ProphetRouter::handle_event(BundleEvent* event)
{
    dispatch_event(event);
}

void
ProphetRouter::get_routing_state(oasys::StringBuffer* buf)
{
    oracle_->dump_state(buf);
}

bool
ProphetRouter::accept_bundle(Bundle* bundle, int* errp)
{
    return oracle_->accept_bundle(bundle,errp);
}

void
ProphetRouter::handle_bundle_received(BundleReceivedEvent *event)
{
    Bundle* bundle = event->bundleref_.object();
    oracle_->handle_bundle_received(bundle,event->contact_);
}

void
ProphetRouter::handle_bundle_delivered(BundleDeliveredEvent* event)
{
    Bundle* bundle = event->bundleref_.object();
    oracle_->handle_bundle_delivered(bundle);
}

void
ProphetRouter::handle_bundle_expired(BundleExpiredEvent *event)
{
    Bundle* bundle = event->bundleref_.object();
    oracle_->handle_bundle_expired(bundle);
}

void
ProphetRouter::handle_link_created(LinkCreatedEvent* event)
{
    LinkRef link = event->link_;
    ASSERT(link != NULL);
    ASSERT(!link->isdeleted());
	
    // can't do anything with "only" a link, except to police this one ASSERTion
    ASSERT(link->remote_eid().equals(EndpointID::NULL_EID()) == false);
}

void
ProphetRouter::handle_contact_up(ContactUpEvent* event)
{
    ASSERT(event->contact_->link() != NULL);
    ASSERT(!event->contact_->link()->isdeleted());

    // ProphetController demux's which ProphetEncounter handles this contact
    // (or creates a new one)
    oracle_->new_neighbor(event->contact_);
}

void
ProphetRouter::handle_link_available(LinkAvailableEvent* event)
{
    LinkRef link = event->link_;
    ASSERT(link != NULL);
    ASSERT(!link->isdeleted());
    
    if (!link->isopen())
    {
        // request to open link
        actions_->open_link(link);
    }
}

void
ProphetRouter::handle_contact_down(ContactDownEvent* event)
{
    // Let ProphetController know that the contact's off the wire
    oracle_->neighbor_gone(event->contact_);
}

void
ProphetRouter::handle_link_state_change_request(LinkStateChangeRequest* req)
{
    Link::state_t state = Link::state_t(req->state_);
    LinkRef link = req->link_;
    ASSERT(link != NULL);

    if (link->isdeleted() && state != Link::CLOSED) {
        log_debug("ProphetRouter::handle_link_state_change_request: "
                  "link %s already deleted; cannot change link state to %s",
                  link->name(), Link::state_to_str(state));
        return;
    }
	
    // Signals the appropriate ProphetEncounter instance to clear any
    // pending outbound queues
    if (req->old_state_ == Link::BUSY && req->state_ == Link::AVAILABLE)
    {
        oracle_->handle_link_state_change_request(req->contact_);
    }
}

} // namespace dtn
