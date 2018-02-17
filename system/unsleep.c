/* unsleep.c - unsleep */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  unsleep  -  Internal function to remove a process from the sleep
 *		    queue prematurely.  The caller must adjust the delay
 *		    of successive processes.
 *------------------------------------------------------------------------
 */
status	unsleep(
	  pid32		pid		/* ID of process to remove	*/
        )
{
	intmask	mask;			/* Saved interrupt mask		*/
        struct	procent	*prptr;		/* Ptr to process' table entry	*/

	mask = disable();

	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	/* Verify that candidate process is on the sleep queue */

	prptr = &proctab[pid];
	if ((prptr->prstate!=PR_SLEEP) && (prptr->prstate!=PR_RECTIM)) {
		restore(mask);
		return SYSERR;
	}
	struct qnewentry *head= getsleephead();
	while( head!= getsleeptail() && head->process_id != pid)
	{
		head=head->qnext;
	}
	if(head != getsleeptail())
	        head->qnext->qkey += head->qkey;
	/* Increment delay of next process if such a process exists */
	
	getitem(pid);			/* Unlink process from queue */
	restore(mask);
	return OK;
}
