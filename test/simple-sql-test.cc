
#include <stdio.h>
#include "debug/Log.h"
#include "bundling/Bundle.h"

#include "storage/PostgresSQLImplementation.h"
#include "storage/MysqlSQLImplementation.h"

#include "storage/SQLBundleStore.h"




class foo : public SerializableObject, public Logger {
public:
  foo() : Logger ("/test/foo") {} ;
  virtual ~foo() {} ; 
  u_int16_t id ;
  u_int32_t  f1;
  u_int8_t f2 ; 
  bool b1;
  std::string  s1;
  void serialize(SerializeAction* a) ;

  void print() ; 
};

void
foo::print() {

    log_info("this object is %d char is %d  and id is %d \n",f1,f2,id);


}

void
foo::serialize(SerializeAction* a)
{
  a->process("id", &id);
  a->process("f1", &f1);
  a->process("f2", &f2);
  
}


void 
playsql(int i) {

    const char* database = "dtn";
    
    //  foo o1; o1.id = 771 ; o1.f1 = 123; o1.f2 = 'a'; foo o2;
    
    SQLImplementation *db ;
    
    if (i ==1)
       db  =  new PostgresSQLImplementation(database);
    else
	db =  new MysqlSQLImplementation(database);

    db->exec_query("drop table try;");
    
    BundleStore *bstore = new SQLBundleStore(db, "try");
    BundleStore::init(bstore);
    
    
    Bundle o1, o2;
    int id1 = 121;
    int id2 = 555;
   
    o1.source_.set_tuple("bundles://internet/tcp://foo");
    o1.bundleid_ = id1;
    o1.custreq_ = 1;
    o1.fwd_rcpt_ = 1;
    
    o2.source_.set_tuple("bundles://google/tcp://foo");
    o2.bundleid_ =  id2;
    o2.return_rcpt_ = 1;
    
    int retval2 = bstore->put(&o1,id1);
    retval2 = bstore->put(&o2,id2);
    
    Bundle *g1 = bstore->get(id1);
    Bundle *g2 = bstore->get(id2);
    
    retval2 = bstore->put(g1,id1);
    
    ASSERT(o1.bundleid_ == g1->bundleid_);
    ASSERT(o1.custreq_ == g1->custreq_);
    ASSERT(o1.custody_rcpt_ == g1->custody_rcpt_);
    ASSERT(o1.recv_rcpt_ == g1->recv_rcpt_);
    ASSERT(o1.fwd_rcpt_ == g1->fwd_rcpt_);
    ASSERT(o1.return_rcpt_ == g1->return_rcpt_);
    ASSERT (o2.bundleid_ == g2->bundleid_);
    
    
    
    db->close();
    
    exit(0);
    
}  

int
main(int argc, const char** argv)
{


    Log::init(LOG_DEBUG);
    playsql(atoi(argv[1]));
    Bundle b, b2;
    b.bundleid_ = 100;
    ASSERT(!b.source_.valid());
    ASSERT(!b2.source_.valid());
    
    b.source_.set_tuple("bundles://internet/tcp://foo");
    ASSERT(b.source_.valid());
    ASSERT(b.source_.region().compare("internet") == 0);
    ASSERT(b.source_.admin().compare("tcp://foo") == 0);

}
