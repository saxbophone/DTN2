
#include "StorageConfig.h"

StorageConfig StorageConfig::instance_;

StorageConfig::StorageConfig()
{
    type_	= "berkeleydb";
    tidy_	= true;
    tidy_wait_	= 3;
    dbdir_	= "/var/bundles/db";
    dbfile_ 	= "DTN.db";
    dberrlog_	= "error.log";
    sqldb_ 	= "DTN";
}
