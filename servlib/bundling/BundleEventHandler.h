/*
 * IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. By
 * downloading, copying, installing or using the software you agree to
 * this license. If you do not agree to this license, do not download,
 * install, copy or use the software.
 * 
 * Intel Open Source License 
 * 
 * Copyright (c) 2005 Intel Corporation. All rights reserved. 
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
#ifndef _BUNDLE_EVENT_HANDLER_H_
#define _BUNDLE_EVENT_HANDLER_H_

#include <oasys/debug/Log.h>

#include "BundleEvent.h"

namespace dtn {

/**
 * Both the BundleDaemon and all the BundleRouter classes need to
 * process the various types of BundleEvent that are generated by the
 * rest of the system. This class provides that abstraction plus a
 * useful dispatching function for event-specific handlers.
 */
class BundleEventHandler : public oasys::Logger {
public:
    /**
     * Pure virtual event handler function.
     */
    virtual void handle_event(BundleEvent* event) = 0;

protected:
    /**
     * Constructor -- protected since this class shouldn't ever be
     * instantiated directly.
     */
    BundleEventHandler(const char* logpath = "/event/handler")
        : oasys::Logger(logpath) {}
    
    /**
     * Destructor -- Needs to be defined virtual to be sure that
     * derived classes get a chance to clean up their stuff on removal.
     */
    virtual ~BundleEventHandler() {}

    /** 
     * Dispatch the event by type code to one of the event-specific
     * handler functions below.
     */
    void dispatch_event(BundleEvent* event);
    
    /**
     * Default event handler for new bundle arrivals.
     */
    virtual void handle_bundle_received(BundleReceivedEvent* event);
    
    /**
     * Default event handler when bundles are transmitted.
     */
    virtual void handle_bundle_transmitted(BundleTransmittedEvent* event);
    
    /**
     * Default event handler when bundles are locally delivered.
     */
    virtual void handle_bundle_delivered(BundleDeliveredEvent* event);
    
    /**
     * Default event handler when bundles expire.
     */
    virtual void handle_bundle_expired(BundleExpiredEvent* event);

    /**
     * Default event handler when bundles are free (i.e. no more
     * references).
     */
    virtual void handle_bundle_free(BundleFreeEvent* event);

    /**
     * Default event handler when a new application registration
     * arrives.
     */
    virtual void handle_registration_added(RegistrationAddedEvent* event);
    
    /**
     * Default event handler when a registration is removed.
     */
    virtual void handle_registration_removed(RegistrationRemovedEvent* event);
    
    /**
     * Default event handler when a registration expires.
     */
    virtual void handle_registration_expired(RegistrationExpiredEvent* event);
    
    /**
     * Default event handler when a new contact is up.
     */
    virtual void handle_contact_up(ContactUpEvent* event);
    
    /**
     * Default event handler when a contact is down.
     */
    virtual void handle_contact_down(ContactDownEvent* event);

    /**
     * Default event handler when a new link is created.
     */
    virtual void handle_link_created(LinkCreatedEvent* event);
    
    /**
     * Default event handler when a link is deleted.
     */
    virtual void handle_link_deleted(LinkDeletedEvent* event);

    /**
     * Default event handler when link becomes available
     */
    virtual void handle_link_available(LinkAvailableEvent* event);    

    /**
     * Default event handler when a link is unavailable
     */
    virtual void handle_link_unavailable(LinkUnavailableEvent* event);

    /**
     * Default event handler for link state change requests.
     */
    virtual void handle_link_state_change_request(LinkStateChangeRequest* req);

    /**
     * Default event handler when reassembly is completed.
     */
    virtual void handle_reassembly_completed(ReassemblyCompletedEvent* event);
    
    /**
     * Default event handler when a new route is added by the command
     * or management interface.
     */
    virtual void handle_route_add(RouteAddEvent* event);
    
    /**
     * Default event handler for shutdown requests.
     */
    virtual void handle_shutdown_request(ShutdownRequest* event);
};

} // namespace dtn

#endif /* _BUNDLE_EVENT_HANDLER_H_ */
