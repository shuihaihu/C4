#ifndef ns_kvstoreapp_h
#define ns_kvstoreapp_h

#include "timer-handler.h"
#include "packet.h"
#include "app.h"


// This is used for client to send a request
struct request_send_accounting { 
        int request_id   //id of the request
        int request_total_bytes  //total bytes of the request to send
        int send_bytes;  // bytes already sent
};


// This is used for server to receive a request
struct request_receive_accounting { 
        int request_id   //id of the request
        int request_total_bytes  //total bytes of the request to be received
        int recv_bytes;  // bytes already received
};


// This is used for server to send a response
struct response_send_accounting { 
        int request_id   //id of the request to response
        int response_total_bytes  //total bytes of the response to send
        int send_bytes;  // bytes already sent
};


// This is used for client to receive a response
struct response_receive_accounting { 
        int request_id   //id of the request to response
        int response_total_bytes  //total bytes of the request to be received
        int recv_bytes;  // bytes already received
};

class KVStoreApp;

// Client uses this timer to 
// schedule next request data packet transmission time
class ClientTimer : public TimerHandler {
 public:
	ClientTimer(KVStoreApp* t) : TimerHandler(), t_(t) {}
	inline virtual void expire(Event*);
 protected:
	KVStoreApp* t_;
};


// Server uses this timer to schedule
// next ack packet transmission time
class ServerTimer : public TimerHandler {
 public:
	ServerTimer(KVStoreApp* t) : TimerHandler(), t_(t) {}
	inline virtual void expire(Event*);
 protected:
	KVStoreApp* t_;
};


// Key-Value Store Application Class Definition
class KVStoreApp : public Application {
 public:
	KVStoreApp();
  void schedule_request(int id, int bytes); //to schedule the sending of a request
  void schedule_response(int id, int bytes); //to schedule the sending of a response
  void send_request_pkt(); // to send one pkt of the request in the header of request queue
  void send_response_pkt(); // to send one pkt of the response in the header of request queue

 protected:
	int command(int argc, const char*const* argv);
 private:
	void init();
  void start();
  void stop();
	inline double next_snd_time();                          
	virtual void recv_msg(int nbytes, const char *msg = 0); 
	void account_recv_pkt(const hdr_kvstore *mh_buf);        
	void init_recv_pkt_accounting();         

	struct request_send_accounting requeststosend[MAX_REQUEST_NUM];

  struct request_recv_accounting requeststorecv[MAX_RESPONSE_NUM];
  struct response_send_accounting responsestosend[MAX_RESPONSE_NUM];
  struct response_recv_accounting responsestorecv[MAX_REQUEST_NUM];
  double machine_capacity_;
	double interval_;      // Request packet transmission interval
	int pktsize_;          // Request data packet size
	int running_;          // If 1 application is running
	ClientTimer client_timer_;  // Client Timer
	ServerTimer  server_timer_;  // Server Timer
};



