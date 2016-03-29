#ifndef ns_dwrr_h
#define ns_dwrr_h

#include "queue.h"
#include "config.h"
#include "trace.h"
#include "timer-handler.h"

/*Maximum queue number */
#define MAX_QUEUE_NUM 64

/* Per-queue ECN marking */
#define PER_QUEUE_MARKING 0
/* Per-port ECN marking */
#define PER_PORT_MARKING 1
/* MQ-ECN for any packet scheduling algorithms */
#define MQ_MARKING_GENER 2
/* MQ-ECN for round robin packet scheduling algorithms */
#define MQ_MARKING_RR 3
/* Dequeue latency-based ECN marking */
#define LATENCY_MARKING 4
/* PIE-like ECN marking */
#define PIE_MARKING 5

#define DQ_COUNT_INVALID -1

class PacketDWRR;
class DWRR;

class DWRR_Timer : public TimerHandler
{
public:
	DWRR_Timer(DWRR *q) : TimerHandler() { queue_=q;}

protected:
	virtual void expire(Event *e);
	DWRR *queue_;
};

class PacketDWRR: public PacketQueue
{
	public:
		PacketDWRR(): quantum(1500), deficitCounter(0), thresh(0), active(false), current(false), start_time(0), dq_tstamp(0), dq_count(DQ_COUNT_INVALID), avg_dq_rate(-1), next(NULL) {}

		int id;	//queue ID
		int quantum;	//quantum (weight) of this queue
		int deficitCounter;	//deficit counter for this queue
		double thresh;	// per-queue ECN marking threshold (pkts)
		bool active;	//whether this queue is active (qlen>0)
		bool current;	//whether this queue is currently being served (deficitCounter has been updated for thie round)
		double start_time;	//time when this queue is inserted to active list
		double dq_tstamp;	//measurement start time
		int dq_count;	//measured in bytes
		double avg_dq_rate;	//average drain rate (bps)
		PacketDWRR *next;	//pointer to next node

		friend class DWRR;
};

class DWRR : public Queue
{
	public:
		DWRR();
		~DWRR();
		void timeout(int);
		virtual int command(int argc, const char*const* argv);

	protected:
		Packet *deque(void);
		void enque(Packet *pkt);
		int TotalByteLength();	//Get total length of all queues in bytes
		int TotalLength();	//Get total length of all queues in packets
		int TotalQuantum();	//Get sum of quantum
		int MarkingECN(int q); //Determine whether we need to mark ECN, q is current queue number

		/* Variables */
		PacketDWRR *queues;	//underlying multi-FIFO (CoS) queues
		PacketDWRR *activeList;	//list for active queues
		DWRR_Timer timer_;	//Underlying timer for quantum_sum_estimate update
		double round_time;	//estimation value for round time
		double quantum_sum_estimate;	//estimation value for sum of quantums of all non-empty  queues
		int quantum_sum;	//sum of quantums of all non-empty queues in activeList
		double last_update_time;	//last time when we update quantum_sum_estimate
		double last_idle_time;	//Last time when link becomes idle
		int init;	//whether the timer has been started

		int queue_num_;	//number of queues
		int mean_pktsize_;	//MTU in bytes
		double port_thresh_;	//per-port ECN marking threshold (pkts)
		int marking_scheme_;	//ECN marking policy
		double estimate_round_alpha_;	//factor between 0 and 1 for round time estimation
		int estimate_round_idle_interval_bytes_;	//Time interval (divided by link capacity) to update round time when link is idle.
		double estimate_quantum_alpha_;	//factor between 0 and 1 for quantum estimation
		int estimate_quantum_interval_bytes_;	//Time interval is estimate_quantum_interval_bytes_/link capacity.
		int estimate_quantum_enable_timer_;	//Whether we use real timer (TimerHandler) for quantum estimation
		int dq_thresh_;	//threshold for departure rate estimation
		double estimate_rate_alpha_;	//factor between 0 and 1 for departure rate estimation
		double link_capacity_;	//Link capacity
		int deque_marking_;	//shall we enable dequeue ECN/RED marking
		int debug_;	//debug more(true) or not(false)

		Tcl_Channel total_qlen_tchan_;	//place to write total_qlen records
		Tcl_Channel qlen_tchan_;	//place to write per-queue qlen records
		void trace_total_qlen();	//routine to write total qlen records
		void trace_qlen();	//routine to write per-queue qlen records
};

#endif
