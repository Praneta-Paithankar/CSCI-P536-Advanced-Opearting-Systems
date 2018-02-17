/* queue.h - firstid, firstkey, isempty, lastkey, nonempty		*/

/* Queue structure declarations, constants, and inline functions	*/

/* Default # of queue entries: 1 per process 	*/
/*			plus 2 per semaphore		*/
#ifndef NQENT
#define NQENT	(NPROC + NSEM + NSEM)
#endif

#define	EMPTY	(-1)		/* Null value for qnext or qprev index	*/
#define	MAXKEY	0x7FFFFFFF	/* Max key that can be stored in queue	*/
#define	MINKEY	0x80000000	/* Min key that can be stored in queue	*/

struct	qentry	{		/* One per process plus two per list	*/
	int32	qkey;		/* Key on which the queue is ordered	*/
	qid16	qnext;		/* Index of next process or tail	*/
	qid16	qprev;		/* Index of previous process or head	*/
};

struct qnewentry{              /* new structure to store processes in queue */
	int32 qkey;		/* key on which the queue is ordered */
	pid32 process_id;	/* process id */
	struct qnewentry* qnext;	/* index of next process or tail */
	struct qnewentry* qprev;	/* index of previous process or head */
};

extern	struct qentry	queuetab[];

struct qnewentry  *queuearr[4]; /*array to store head and tail of ready queue and sleep queue*/

/* Inline queue manipulation functions */

#define	queuehead(q)	(q)
#define	queuetail(q)	((q) + 1)
#define	firstid(q)	(queuetab[queuehead(q)].qnext)
#define	lastid(q)	(queuetab[queuetail(q)].qprev)
#define	isempty(q)	(firstid(q) >= NPROC)
#define	nonempty(q)	(firstid(q) <  NPROC)
#define	firstkey(q)	(queuetab[firstid(q)].qkey)
#define	lastkey(q)	(queuetab[ lastid(q)].qkey)
#define getreadyhead()	queuearr[0]	/*queuearr[0] stores head of ready queue*/
#define getreadytail()	queuearr[1]
#define getsleephead()	queuearr[2]
#define getsleeptail()	queuearr[3]





/* Inline to check queue id assumes interrupts are disabled */

#define	isbadqid(x)	(((int32)(x) < 0) || (int32)(x) >= NQENT-1)
