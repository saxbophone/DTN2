/*
 *    Copyright 2004-2006 Intel Corporation
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

#ifndef _CONTACT_MANAGER_H_
#define _CONTACT_MANAGER_H_

#include <oasys/debug/Log.h>
#include <oasys/thread/SpinLock.h>
#include <oasys/thread/Timer.h>
#include <oasys/util/StringBuffer.h>
#include <oasys/util/StringUtils.h>
#include "bundling/BundleEventHandler.h"

namespace dtn {

class Contact;
class ConvergenceLayer;
class CLInfo;
class Link;
class LinkSet;

/**
 * A contact manager class. Maintains topological information and
 * connectivity state regarding available links and contacts.
 */
class ContactManager : public BundleEventHandler {
public:
    /**
     * Constructor / Destructor
     */
    ContactManager();
    virtual ~ContactManager();
    
    /**
     * Dump a string representation of the info inside contact manager.
     */
    void dump(oasys::StringBuffer* buf) const;
    
    /**********************************************
     *
     * Link set accessor functions
     *
     *********************************************/
    /**
     * Add a link.
     */
    void add_link(const LinkRef& link);
    
    /**
     * Delete a link
     */
    void del_link(const LinkRef& link);
    
    /**
     * Check if contact manager already has this link
     */
    bool has_link(const LinkRef& link);

    /**
     * Finds link corresponding to this name
     */
    LinkRef find_link(const char* name);

    /**
     * Helper routine to find a link based on criteria:
     *
     * @param cl 	 The convergence layer
     * @param nexthop	 The next hop string
     * @param remote_eid Remote endpoint id (NULL_EID for any)
     * @param type	 Link type (LINK_INVALID for any)
     * @param states	 Bit vector of legal link states, e.g. ~(OPEN | OPENING)
     *
     * @return The link if it matches or NULL if there's no match
     */
    LinkRef find_link_to(ConvergenceLayer* cl,
                         const std::string& nexthop,
                         const EndpointID& remote_eid = EndpointID::NULL_EID(),
                         Link::link_type_t type = Link::LINK_INVALID,
                         u_int states = 0xffffffff);
    
    /**
     * Return the list of links. Asserts that the CM spin lock is held
     * by the caller.
     */
    const LinkSet* links();

    /**
     * Accessor for the ContactManager internal lock.
     */
    oasys::Lock* lock() { return &lock_; }

    /**********************************************
     *
     * Event handler routines
     *
     *********************************************/
    /**
     * Generic event handler.
     */
    void handle_event(BundleEvent* event)
    {
        dispatch_event(event);
    }
    
    /**
     * Event handler when a link becomes unavailable.
     */
    void handle_link_available(LinkAvailableEvent* event);
    
    /**
     * Event handler when a link becomes unavailable.
     */
    void handle_link_unavailable(LinkUnavailableEvent* event);

    /**
     * Event handler when a link is opened successfully
     */
    void handle_contact_up(ContactUpEvent* event);

    /**********************************************
     *
     * Opportunistic contact routines
     *
     *********************************************/
    /**
     * Notification from a convergence layer that a new opportunistic
     * link has come knocking.
     *
     * @return An idle link to represent the new contact
     */
    LinkRef new_opportunistic_link(ConvergenceLayer* cl,
                                   const std::string& nexthop,
                                   const EndpointID& remote_eid,
                                   const std::string* link_name = NULL);
    
protected:
    
    LinkSet* links_;			///< Set of all links
    int opportunistic_cnt_;		///< Counter for opportunistic links

    /**
     * Reopen a broken link.
     */
    void reopen_link(const LinkRef& link);

    /**
     * Timer class used to re-enable broken ondemand links.
     */
    class LinkAvailabilityTimer : public oasys::Timer {
    public:
        LinkAvailabilityTimer(ContactManager* cm, const LinkRef& link)
            : cm_(cm), link_(link.object(), "LinkAvailabilityTimer") {}
        
        virtual void timeout(const struct timeval& now);

        ContactManager* cm_;	///< The contact manager object
        LinkRef link_;		///< The link in question
    };
    friend class LinkAvailabilityTimer;

    /**
     * Table storing link -> availability timer class.
     */
    typedef std::map<LinkRef, LinkAvailabilityTimer*> AvailabilityTimerMap;
    AvailabilityTimerMap availability_timers_;

    /**
     * Lock to protect internal data structures.
     */
    mutable oasys::SpinLock lock_;
};


} // namespace dtn

#endif /* _CONTACT_MANAGER_H_ */
