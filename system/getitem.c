/* getitem.c - getfirst, getlast, getitem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getfirst  -  Remove a process from the front of a queue
 *------------------------------------------------------------------------
 */
pid32	getfirst(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	pid32	head;

	if (isempty(q)) {
		return EMPTY;
	}
        if(q ==readylist || q==sleepq)
	{
		struct qnewentry *curr,*qhead;
		if(q == readylist){ 
			qhead=getreadyhead();
		}
		else{
			qhead=getsleephead();
		}
		curr=qhead->qnext;
		curr->qnext->qprev=qhead;
		qhead->qnext=curr->qnext;
		curr->qnext=NULL;
		curr->qprev=NULL;
		freemem((char * )curr,sizeof(struct qnewentry ));
		
	}
	head = queuehead(q);
	return getitem(queuetab[head].qnext);
}

/*------------------------------------------------------------------------
 *  getlast  -  Remove a process from end of queue
 *------------------------------------------------------------------------
 */
pid32	getlast(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	pid32 tail;

	if (isempty(q)) {
		return EMPTY;
	}
	
	if(q==readylist || q==sleepq)
	{
		struct qnewentry *curr,*qtail;
		if(q == readylist) 
		{
			qtail=getreadytail();
		}
		else
		{
			qtail=getsleeptail();
		}
		curr=qtail->qprev;
		curr->qprev->qnext=qtail;
		qtail->qprev=curr->qprev;
		curr->qnext=NULL;
		curr->qprev=NULL;
		freemem((char * )curr,sizeof(struct qnewentry ));
		
	}
	tail = queuetail(q);
	return getitem(queuetab[tail].qprev);
}

/*------------------------------------------------------------------------
 *  getitem  -  Remove a process from an arbitrary point in a queue
 *------------------------------------------------------------------------
 */
pid32	getitem(
	  pid32		pid		/* ID of process to remove	*/
	)
{
	pid32	prev, next;
	if(proctab[pid].prstate==PR_READY || proctab[pid].prstate==PR_SLEEP)
	{
		struct qnewentry *head,*tail;
		if(proctab[pid].prstate==PR_READY)
		{
			head=getreadyhead();
			tail=getreadytail();
		}
		else
		{
			tail=getsleeptail();
			head=getsleephead();
		}
		while(head!=tail && head->process_id==pid)
		{
			head=head->qnext;
		}
		if (head == tail) 
			return SYSERR;
		head->qprev->qnext=head->qnext;
		head->qnext->qprev=head->qprev;
		return pid;
	}
	next = queuetab[pid].qnext;	/* Following node in list	*/
	prev = queuetab[pid].qprev;	/* Previous node in list	*/
	queuetab[prev].qnext = next;
	queuetab[next].qprev = prev;
	return pid;
}
