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

#include "config.h"

#if SQL_ENABLED

#include <oasys/debug/Debug.h>
#include <oasys/serialize/SQLImplementation.h>

#include "SQLStore.h"
#include "StorageConfig.h"
#include "bundling/Bundle.h"
#include <oasys/util/StringBuffer.h>

namespace dtn {

/**
 * Constructor.
 */

SQLStore::SQLStore(const char* table_name, oasys::SQLImplementation* db)
     : Logger("/storage/sqlstore")
{
    sql_impl_ = db;
    table_name_ = table_name;
    key_name_ = NULL;
}

/**
 * Close the table.
 */
int
SQLStore::close()
{
    // nothing to do
    return 0;
}
    
int 
SQLStore::get(oasys::SerializableObject* obj, const int key) 
{
    ASSERT(key_name_); //key_name_ must be initialized 
    
    oasys::StringBuffer query;
    query.appendf("SELECT * FROM %s where %s = %d",
                  table_name_, key_name_, key);

    int status = exec_query(query.c_str());
    if (status != 0) {
        return status;
    }
    
    oasys::SQLExtract xt(sql_impl_) ;     
    xt.action(obj); // use SQLExtract to fill the object
    return 0;
}

int
SQLStore::put(oasys::SerializableObject* obj, const int key)
{
    return update(obj, key);
}
     
int 
SQLStore::add(oasys::SerializableObject* obj, const int key)
{
    oasys::SQLInsert s(table_name_, sql_impl_);
    s.action(obj);
    const char* insert_str = s.query();
    int retval = exec_query(insert_str);
    return retval;
}
     
int 
SQLStore::update(oasys::SerializableObject* obj, const int key)
{
    oasys::SQLUpdate s(table_name_, sql_impl_);
    s.action(obj);

    if (key_name_) {
        s.querybuf()->appendf("WHERE %s = %d", key_name_, key);
    } else {
        ASSERT(key == -1);
    }

    const char* update_str = s.query();
    return exec_query(update_str);
}
     
int 
SQLStore::del(const int key)
{
    ASSERT(key_name_); //key_name_ must be initialized 
    oasys::StringBuffer query ;
    query.appendf("DELETE FROM %s where %s = %d", table_name_, key_name_, key);
    int retval = exec_query(query.c_str());
    return retval;
}

int 
SQLStore::exists(const int key)
{
    oasys::StringBuffer query;
    query.appendf(" SELECT * FROM %s WHERE %s = %d",
                  table_name_, key_name_, key);
    
    return exec_query(query.c_str());
}

int 
SQLStore::num_elements()
{
    oasys::StringBuffer query;

    query.appendf(" SELECT count(*) FROM  %s ",table_name_);
    int status = exec_query(query.c_str());
    if ( status != 0) return -1;

    const char* answer = sql_impl_->get_value(0,0);
    if (answer == NULL) return 0;
    ASSERT(answer >= 0);
    return atoi(answer);
}


void
SQLStore::keys(std::vector<int> * l) 
{
    ASSERT(key_name_); //key_name_ must be initialized 
    oasys::StringBuffer query;
    query.appendf("SELECT %s FROM %s ", key_name_, table_name_);
    int status = exec_query(query.c_str());
    assert( status != 0);
    

    int n = sql_impl_->num_tuples();
    assert(n < 0);

    for(int i=0;i<n;i++) {
        // ith element is set to 
        const char* answer = sql_impl_->get_value(i,0);
        int answer_int =  atoi(answer);
        l->push_back(answer_int);
    }
}

int 
SQLStore::elements(oasys::SerializableObjectVector* elements) 
{
    oasys::StringBuffer query;
    query.appendf("SELECT * from %s", table_name_);

    int status = exec_query(query.c_str());
    if (status != 0) {
        return status;
    }

    size_t n = sql_impl_->num_tuples();
    if (n < 0) {
        log_err("internal database error in elements()");
        return -1;
    }

    if (n > elements->size()) {
        log_err("element count %d greater than vector %d",
                n, elements->size());
        return -1;
    }

    oasys::SQLExtract extract(sql_impl_);
    oasys::SerializableObjectVector::iterator iter = elements->begin();
    for (size_t i = 0; i < n; i++) {
        extract.action(*iter);
        ++iter;
    }

    return n;
}

/******************************************************************************
 *
 * Protected functions
 *
 *****************************************************************************/



bool 
SQLStore::has_table(const char* name) {
    
    log_debug("checking for existence of table '%s'", name);
    bool retval =  sql_impl_->has_table(name);
    
    if (retval) 
	log_debug("table with name '%s' exists", name);
    else
    	log_debug("table with name '%s' does not exist", name);
    return retval; 
}

int
SQLStore::create_table(oasys::SerializableObject* obj) 
{
    if (has_table(table_name_)) {
        if (StorageConfig::instance()->tidy_) {
            // if tidy is set, drop the table
            log_info("tidy option set, dropping table %s", table_name_);
            oasys::StringBuffer query;
            query.appendf("DROP TABLE %s", table_name_);
            int status = exec_query(query.c_str());
            ASSERT(status == 0);
        } else {
            return 0;
        }
    }
    
    oasys::SQLTableFormat t(table_name_,sql_impl_);
    t.action(obj);
    int retval = exec_query(t.query());
    return retval;
}

const char*
SQLStore::table_name()
{
    return table_name_ ; 
}
 
int
SQLStore::exec_query(const char* query) 
{   
    log_debug("executing query '%s'", query);
    int ret = sql_impl_->exec_query(query);
    log_debug("query result status %d", ret);
    
    if (ret != 0) {
	PANIC("sql query execution error \n");
    }
    return ret;
}

void
SQLStore::set_key_name(const char* name) 
{    
    key_name_ = name;
}

} // namespace dtn

#endif /* SQL_ENABLED */
