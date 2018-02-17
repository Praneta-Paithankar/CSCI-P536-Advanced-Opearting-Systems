/* queue.c - enqueue, dequeue */

#include <xinu.h>

struct qentry	queuetab[NQENT];	/* Table of process queues	*/

/*------------------------------------------------------------------------
 *  enqueue  -  Insert a process at the tail of a queue
 *------------------------------------------------------------------------
 */
pid32	enqueue(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q		/* ID of queue to use		*/
	)
{
	qid16	tail, prev;		/* Tail & previous node indexes	*/

	if (isbadpid(pid)) {
		return SYSERR;
	}
	if(q==readylist || q== sleepq)
	{
		struct qnewentry * n=(struct qnewentry *) getmem(sizeof(struct qnewentry));
		n->process_id=pid;
		n->qkey=proctab[pid].prprio;
		struct qnewentry * tailnode=NULL;
		if(q==readylist)
		{
			tailnode=getreadytail();
		}
		else
		{
			tailnode=getsleeptail();
		}
		tailnode->qprev->qnext=n;
		n->qprev=tailnode->qprev;
		n->qnext=tailnode;
		tailnode->qprev=n;
		return pid;
	}
    if(isbadqid(q))
    {
        return SYSERR;
    }
	tail = queuetail(q);
	prev = queuetab[tail].qprev;

	queuetab[pid].qnext  = tail;	/* Insert just before tail node	*/
	queuetab[pid].qprev  = prev;
	queuetab[prev].qnext = pid;
	queuetab[tail].qprev = pid;
	return pid;
}

/*------------------------------------------------------------------------
 *  dequeue  -  Remove and return the first process on a list
 *------------------------------------------------------------------------
 */
pid32	dequeue(
	  qid16		q		/* ID queue to use		*/
	)
{
	pid32	pid;			/* ID of process removed	*/
	if(q==readylist || q== sleepq)
	{
		struct qnewentry *headnode,*curr;
		if(q==readylist)
		{
			headnode=getreadyhead();
		}
		else
		{
			headnode=getsleephead();
		}
		curr=headnode->qnext;
		curr->qnext->qprev=headnode;
		headnode->qnext=curr->qnext;
		curr->qnext=NULL;
		curr->qprev=NULL;
		pid=curr->process_id;
		freemem((char *)curr,sizeof(struct qnewentry));
		return pid;
	}
	if (isbadqid(q)) {
		return SYSERR;
	} else if (isempty(q)) {
		return EMPTY;
	}
	pid = getfirst(q);
	queuetab[pid].qprev = EMPTY;
	queuetab[pid].qnext = EMPTY;
	return pid;
}
