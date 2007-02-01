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

#ifndef _CONVERGENCE_LAYER_H_
#define _CONVERGENCE_LAYER_H_

#include <string>
#include <vector>
#include <oasys/util/Singleton.h>
#include <oasys/serialize/Serialize.h>

#include "contacts/Link.h"
#include "contacts/Contact.h"
#include "contacts/Interface.h"

// forward declaration
namespace oasys {
class StringBuffer;
}

namespace dtn {

/**
 * The abstract interface for a convergence layer.
 */
class ConvergenceLayer : public oasys::Logger {
public:
    /**
     * Constructor.
     */
    ConvergenceLayer(const char* classname,
                     const char* name)
        : Logger(classname, "/dtn/cl/%s", name),
          name_(name)
    {
    }

    /**
     * Destructor.
     */
    virtual ~ConvergenceLayer();

    /**
     * Set default interface options.
     */
    virtual bool set_interface_defaults(int argc, const char* argv[],
                                        const char** invalidp);
    
    /**
     * Set default link options.
     */
    virtual bool set_link_defaults(int argc, const char* argv[],
                                   const char** invalidp);
    
    /**
     * Bring up a new interface.
     */
    virtual bool interface_up(Interface* iface,
                              int argc, const char* argv[]);

    /**
     * Bring down the interface.
     */
    virtual bool interface_down(Interface* iface);

    /**
     * Dump out CL specific interface information.
     */
    virtual void dump_interface(Interface* iface, oasys::StringBuffer* buf);

    /**
     * Create any CL-specific components of the Link.
     */
    virtual bool init_link(const LinkRef& link, int argc, const char* argv[]);

    /**
     * Delete any CL-specific components of the link (requests pertaining
     * to this link must be ignored gracefully by the CL in the future).
     */
    virtual void delete_link(const LinkRef& link);

    /**
     * Dump out CL specific link information.
     */
    virtual void dump_link(const LinkRef& link, oasys::StringBuffer* buf);

    /**
     * Post-initialization, parse any CL-specific options for the link.
     */
    virtual bool reconfigure_link(const LinkRef& link,
                                  int argc, const char* argv[]);

    /**
     * Open a new contact for the given link. The implementation will
     * create a new Contact object (or find one that already exists),
     * establish any CL specific connections, then post a
     * ContactUpEvent when the contact is successfully initiated.
     */
    virtual bool open_contact(const ContactRef& contact) = 0;
    
    /**
     * Close the open contact.
     *
     * Mainly used to clean the state that is associated with this
     * contact. This is called by the link->close() function.
     *
     * Note that this function should NOT post a ContactDownEvent, as
     * this function is only called to clean up the contact state
     * after it has been closed (i.e. after the ContactDownEvent has
     * been generated by some other part of the system).
     */
    virtual bool close_contact(const ContactRef& contact);

    /**
     * Try to send the given bundle on the current link.
     *
     * In some cases (e.g. TCP) this just sticks bundles on a queue
     * for another thread to consume (after setting the link state to
     * BUSY). In others (e.g. UDP) there is no per-contact thread, so
     * this callback is used to send the bundle.
     */
    virtual void send_bundle(const ContactRef& contact, Bundle* bundle) = 0;
    
    /**
     * Try to cancel transmission of a given bundle on the contact.
     */
    virtual bool cancel_bundle(const ContactRef& contact, Bundle* bundle)
    {
        (void)contact;
        (void)bundle;
        return false;
    }
    
    /**
     * Hook to see if the given bundle is queued for transmission on
     * the given contact.
     */
    virtual bool is_queued(const ContactRef& contact, Bundle* bundle)
    {
        (void)contact;
        (void)bundle;
        return false;
    }
    
    /**
     * Boot-time initialization and registration of statically defined
     * convergence layers.
     */
    static void init_clayers();
    static void add_clayer(ConvergenceLayer* cl);
    
    /**
     * Find the appropriate convergence layer for the given 
     * string.
     */
    static ConvergenceLayer* find_clayer(const char* proto);

    /**
     * Accessor for the convergence layer name.
     */
    const char* name() { return name_; }
    
    /**
     * Magic number used for DTN convergence layers     
     */
    static const u_int32_t MAGIC = 0x64746e21; //'DTN!'
    
protected:
    /**
     * The unique name of this convergence layer.
     */
    const char* name_;

};

/**
 * Use a singleton vector to enumerate the convergence layers that
 * are currently implemented.
 */
class CLVector : public oasys::Singleton<CLVector>,
                 public std::vector<ConvergenceLayer*> {
public:
    virtual ~CLVector();
};
    
/**
 * Abstract base class for convergence layer specific state stored in
 * an interface / contact / link.
 */
class CLInfo : public oasys::SerializableObject {
public:
    virtual ~CLInfo() {}

    /**
     * Virtual from SerializableObject
     */
    virtual void serialize( oasys::SerializeAction *) {}
};

} // namespace dtn

#endif /* _CONVERGENCE_LAYER_H_ */
