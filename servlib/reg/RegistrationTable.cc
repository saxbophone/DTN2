
#include "RegistrationTable.h"
#include "bundling/BundleEvent.h"
#include "debug/Debug.h"
#include "routing/BundleRouter.h"
#include "storage/RegistrationStore.h"

RegistrationTable* RegistrationTable::instance_ = NULL;

RegistrationTable::RegistrationTable(RegistrationStore* store)
    : Logger("/registration")
{
}

RegistrationTable::~RegistrationTable()
{
    NOTREACHED;
}

/**
 * Load in the registration table.
 */
bool
RegistrationTable::load()
{
    RegistrationStore::instance()->load(&reglist_);
    return true;
}


/**
 * Internal method to find the location of the given registration.
 */
bool
RegistrationTable::find(u_int32_t regid, const BundleTuple& endpoint,
                        RegistrationList::iterator* iter)
{
    Registration* reg;

    for (*iter = reglist_.begin(); *iter != reglist_.end(); ++(*iter)) {
        reg = *(*iter);
        
        if ((reg->regid() == regid) && (reg->endpoint().compare(endpoint) == 0)) {
            return true;
        }
    }

    return false;
}

/**
 * Look up a matching registration.
 */
Registration*
RegistrationTable::get(u_int32_t regid, const BundleTuple& endpoint)
{
    RegistrationList::iterator iter;

    if (find(regid, endpoint, &iter)) {
        return *iter;
    }
    return NULL;
}

/**
 * Add a new registration to the database. Returns true if the
 * registration is successfully added, false if there's another
 * registration with the same {endpoint,regid}.
 */
bool
RegistrationTable::add(Registration* reg)
{
    log_info("adding registration %d/%s",
             reg->regid(), reg->endpoint().c_str());
    
    // check if a conflicting registration already exists
    if (get(reg->regid(), reg->endpoint()) != NULL) {
        log_err("error adding registration %d/%s: duplicate registration",
                reg->regid(), reg->endpoint().c_str());
        return false; 
    }
    
    reglist_.push_back(reg);
    
    if (! RegistrationStore::instance()->add(reg)) {
        log_err("error adding registration %d/%s: error in persistent store",
                reg->regid(), reg->endpoint().c_str());
        return false;
    }

    // finally, notify the routing layer
    BundleRouter::dispatch(new RegistrationAddedEvent(reg));

    return true;
}

/**
 * Remove the registration from the database, returns true if
 * successful, false if the registration didn't exist.
 */
bool
RegistrationTable::del(u_int32_t regid, const BundleTuple& endpoint)
{
    RegistrationList::iterator iter;

    log_info("removing registration %d/%s", regid, endpoint.c_str());
    
    if (! find(regid, endpoint, &iter)) {
        log_err("error removing registration %d/%s: no matching registration",
                regid, endpoint.c_str());
        return false;
    }

    if (! RegistrationStore::instance()->del(*iter)) {
        log_err("error removing registration %d/%s: error in persistent store",
                regid, endpoint.c_str());
        return false;
        
    }

    reglist_.erase(iter);

    return true;
}
    
/**
 * Update the registration in the database. Returns true on
 * success, false on error.
 */
bool
RegistrationTable::update(Registration* reg)
{
    log_info("updating registration %d/%s",
             reg->regid(), reg->endpoint().c_str());

    if (! RegistrationStore::instance()->update(reg)) {
        log_err("error updating registration %d/%s: error in persistent store",
                reg->regid(), reg->endpoint().c_str());
        return false;
    }

    return true;
}
    
/**
 * Populate the given reglist with all registrations with an endpoint
 * id that matches the prefix of that in the bundle demux string.
 *
 * Returns the count of matching registrations.
 */
int
RegistrationTable::get_matching(const BundleTuple& demux,
                                RegistrationList* reg_list)
{
    int count = 0;
    
    RegistrationList::iterator iter;
    Registration* reg;

    log_debug("get_matching %s", demux.c_str());
    
    for (iter = reglist_.begin(); iter != reglist_.end(); ++iter) {
        reg = *iter;

        if (reg->endpoint().match(demux)) {
            log_debug("matched registration %d %s",
                      reg->regid(), reg->endpoint().c_str());
            count++;
            reg_list->push_back(reg);
        }
    }

    return count;
}
