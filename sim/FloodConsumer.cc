#include "FloodConsumer.h"
#include "reg/Registration.h"
#include "util/StringUtils.h"

FloodConsumer::FloodConsumer(u_int32_t regid,
                             const BundleTuplePattern& endpoint)
    : Registration(regid, endpoint, Registration::ABORT)
{
    set_active(true);

    log_info("FLOOD_CONSUMER : adding registration at endpoint:%s",
             endpoint.c_str());
}

void
FloodConsumer::consume_bundle(Bundle *bundle)
{
    log_info("FLOOD_CONSUMER : consuming bundle id:%d",bundle->bundleid_);

    //add stats
    //router
}

void
FloodConsumer::set_router(BundleRouter *router)
{
    router_ = router;
}
