//  
// Author:    Shuihai Hu 
// File:      kvstore-app.cc
//   

#include "random.h"
#include "kvstore-app.h"


// KVStore Application OTcl linkage class
static class KVStoreAppClass : public TclClass {
 public:
  KVStoreAppClass() : TclClass("Application/KVStoreApp") {}
  TclObject* create(int, const char*const*) {
    return (new KVStoreApp);
  }
} class_app_kvstore;


// When client_timer_ expires call KVStoreApp:send_request_pkt()
void ClientTimer::expire(Event*)
{
  t_->send_request_pkt();
}


// When server_timer_ expires call KVStoreApp:send_response_pkt()
void ServerTimer::expire(Event*)
{
  t_->send_response_pkt();
}


// Constructor (also initialize instances of timers)
KVStoreApp::KVStoreApp() : running_(0), machine_capacity_(1250000000),sender_timer_(this), ack_timer_(this)
{
bind("pktsize_", &pktsize_);
}


// OTcl command interpreter
int KVStoreApp::command(int argc, const char*const* argv)
{
  Tcl& tcl = Tcl::instance();

  if (argc == 3) {
    if (strcmp(argv[1], "attach-agent") == 0) {
      agent_ = (Agent*) TclObject::lookup(argv[2]);
      if (agent_ == 0) {
	tcl.resultf("no such agent %s", argv[2]);
	return(TCL_ERROR);
      }

      
      agent_->attachApp(this);
      return(TCL_OK);
    }
  }
  return (Application::command(argc, argv));
}



void KVStoreApp::init()
{
  interval_ = (double)(pktsize_ << 3)/(double)machine_capacity_;
}


void KVStoreApp::start()
{
  init();
  running_ = 1;
}


void KVStoreApp::stop()
{
  running_ = 0;
}


// Send request packet
void KVStoreApp::send_request_pkt()
{

}


// Schedule next data packet transmission time
double KVStoreApp::next_snd_time()
{
  // Recompute interval in case rate or size chages
  interval_ = (double)(pktsize_ << 3)/(double)machine_capacity;
  double next_time_ = interval_;
  return next_time_;
}


// Receive message from underlying agent
void KVStoreApp::recv_msg(int nbytes, const char *msg)
{
  if(msg) {
    hdr_kvstore* mh_buf = (hdr_kvstore*) msg;
    account_recv_pkt(mh_buf);
  }
}



void KVStoreApp::account_recv_pkt(const hdr_mm *mh_buf)
{ 

}


void KVStoreApp::init_recv_pkt_accounting()
{
}


void KVStoreApp::send_response_pkt(void)
{

}

