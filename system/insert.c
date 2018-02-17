/* insert.c - insert */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insert  -  Insert a process into a queue in descending key order
 *------------------------------------------------------------------------
 */
status	insert(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  int32		key		/* Key for the inserted process	*/
	)
{
	int16	curr;			/* Runs through items in a queue*/
	int16	prev;			/* Holds previous node index	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}
	if(q==readylist || q == sleepq) /*insert for readylist and sleepq*/
	{
		struct qnewentry *head,*tail,*n;
		n=(struct qnewentry*) getmem(sizeof(struct qnewentry)); /*create new node*/
		n->process_id=pid;
		n->qkey=key;
		if(q==readylist){
			head=getreadyhead();
			tail=getreadytail();
		}
		else{
			head=getsleephead();
			tail=getsleeptail();
		}
		while(head!=tail && head->qkey>=key) /*find node which has smaller priority*/
		{	
			head=head->qnext;
		}
		head->qprev->qnext=n; /*change links*/
		n->qprev=head->qprev;
		n->qnext=head;
		head->qprev=n;
		return OK;
	}
	curr = firstid(q);
	while (queuetab[curr].qkey >= key) {
		curr = queuetab[curr].qnext;
	}

	/* Insert process between curr node and previous node */

	prev = queuetab[curr].qprev;	/* Get index of previous node	*/
	queuetab[pid].qnext = curr;
	queuetab[pid].qprev = prev;
	queuetab[pid].qkey = key;
	queuetab[prev].qnext = pid;
	queuetab[curr].qprev = pid;
	return OK;
}
