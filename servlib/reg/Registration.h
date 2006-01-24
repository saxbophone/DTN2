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
#ifndef _REGISTRATION_H_
#define _REGISTRATION_H_

#include <list>
#include <string>

#include <oasys/debug/Log.h>
#include <oasys/serialize/Serialize.h>
#include <oasys/thread/Timer.h>

#include "../naming/EndpointID.h"

namespace dtn {

class Bundle;

/**
 * Class used to represent an "application" registration, loosely
 * defined to also include internal router mechanisms that consume
 * bundles.
 *
 * Stored in the RegistrationTable, indexed by regid.
 *
 * Registration state is stored persistently in the database.
 */
class Registration : public oasys::SerializableObject, public oasys::Logger {
public:
    /**
     * Reserved registration identifiers.
     */
    static const u_int32_t ADMIN_REGID = 0;
    static const u_int32_t LINKSTATEROUTER_REGID = 1;
    static const u_int32_t MAX_RESERVED_REGID = 9;
    
    /**
     * Type enumerating the option requested by the registration for
     * how to handle bundles when not connected.
     */
    typedef enum {
        DROP,		///< Drop bundles
        DEFER,		///< Spool bundles until requested
        EXEC		///< Execute the specified callback procedure
    } failure_action_t;

    /**
     * Get a string representation of a failure action.
     */
    static const char* failure_action_toa(failure_action_t action);

    /**
     * Constructor for deserialization
     */
    Registration(const oasys::Builder&);

    /**
     * Constructor.
     */
    Registration(u_int32_t regid,
                 const EndpointIDPattern& endpoint,
                 failure_action_t action,
                 u_int32_t expiration,
                 const std::string& script = "");

    /**
     * Destructor.
     */
    virtual ~Registration();

    /**
     * Abstract hook for subclasses to deliver a bundle to this registration.
     */
    virtual void deliver_bundle(Bundle* bundle) = 0;
    
    //@{
    /// Accessors
    u_int32_t	             regid()	         { return regid_; }
    const EndpointIDPattern& endpoint()          { return endpoint_; } 
    failure_action_t         failure_action()    { return failure_action_; }
    const std::string&       script()            { return script_; }
    u_int32_t	             expiration()	 { return expiration_; }
    bool                     active()            { return active_; }
    void                     set_active(bool a)  { active_ = a; }
    bool                     expired()           { return expired_; }
    void                     set_expired(bool e) { expired_ = e; }
    //@}

    /**
     * Virtual from SerializableObject.
     */
    void serialize(oasys::SerializeAction* a);

protected:
    /**
     * Class to implement registration expirations. Note that we use
     * the registration id and not a pointer to the registration class
     * to mitigate potential race conditions that could lead to a
     * dangling pointer.
     */
    class ExpirationTimer : public oasys::Timer {
    public:
        ExpirationTimer(Registration* reg)
            : reg_(reg) {}

        virtual void timeout(struct timeval* now);
        
        Registration* reg_;
    };

    void init_expiration_timer();
    
    u_int32_t regid_;
    EndpointIDPattern endpoint_;
    failure_action_t failure_action_;	
    std::string script_;
    u_int32_t expiration_;
    ExpirationTimer* expiration_timer_;
    bool active_;    
    bool bound_;    
    bool expired_;    
};

/**
 * Typedef for a list of Registrations.
 */
class RegistrationList : public std::list<Registration*> {};

} // namespace dtn

#endif /* _REGISTRATION_H_ */
