#ifndef _REGISTRATION_STORE_H_
#define _REGISTRATION_STORE_H_

#include <string>
#include "reg/Registration.h"

/**
 * Abstract base class for the persistent registration store.
 */
class RegistrationStore {
public:
    /**
     * Singleton instance accessor.
     */
    static RegistrationStore* instance() {
        if (instance_ == NULL) {
            PANIC("RegistrationStore::init not called yet");
        }
        return instance_;
    }

    /**
     * Boot time initializer that takes as a parameter the actual
     * instance to use.
     */
    static void init(RegistrationStore* instance) {
        if (instance_ != NULL) {
            PANIC("RegistrationStore::init called multiple times");
        }
        instance_ = instance;
    }

    /**
     * Return true if initialization has completed.
     */
    static bool initialized() { return (instance_ != NULL); }
    
    /**
     * Load in the whole database of registrations, populating the
     * given list.
     */
    virtual void load(RegistrationList* reg_list) = 0;

    /**
     * Add a new registration to the database. Returns true if the
     * registration is successfully added, false on error.
     */
    virtual bool add(Registration* reg) = 0;
    
    /**
     * Remove the registration from the database, returns true if
     * successful, false on error.
     */
    virtual bool del(Registration* reg) = 0;
    
    /**
     * Update the registration in the database. Returns true on
     * success, false if there's no matching registration or on error.
     */
    virtual bool update(Registration* reg) = 0;

protected:
    static RegistrationStore* instance_;
};

#endif /* _REGISTRATION_STORE_H_ */
