#ifndef _BUNDLE_CONSUMER_H_
#define _BUNDLE_CONSUMER_H_

#include "debug/Log.h"

class Bundle;
class BundleList;
class BundleMapping;
class BundleTuple;

/**
 * Base class used for "next hops" in the bundle forwarding logic, i.e
 * either a local registration or a Contact/Link/Peer.
 */
class BundleConsumer : public Logger {
public:
    /**
     * Constructor. It is the responsibility of the subclass to
     * allocate the bundle_list_ if the consumer does any queuing.
     */
    BundleConsumer(const BundleTuple* dest_tuple, bool is_local);
    
    /**
     * Add the bundle to the queue.
     */
    virtual void enqueue_bundle(Bundle* bundle, const BundleMapping* mapping);

    /**
     * Attempt to remove the given bundle from the queue.
     *
     * @return true if the bundle was dequeued, false if not. If
     * mappingp is non-null, return the old mapping as well.
     */
    virtual bool dequeue_bundle(Bundle* bundle, BundleMapping** mappingp);

    /**
     * Check if the given bundle is already queued on this consumer.
     */
    virtual bool is_queued(Bundle* bundle);

    /**
     * Each BundleConsumer has a tuple (either the registration
     * endpoint or the next-hop address).
     */
    const BundleTuple* dest_tuple() { return dest_tuple_; }

    /**
     * Is the consumer a local registration or a peer.
     */
    bool is_local() { return is_local_; }
    
protected:
    const BundleTuple* dest_tuple_;
    bool is_local_;
    BundleList* bundle_list_;

private:
    /**
     * Default constructor should not be used.
     */
    BundleConsumer();
};

#endif /* _BUNDLE_CONSUMER_H_ */
