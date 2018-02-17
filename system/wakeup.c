/* wakeup.c - wakeup */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wakeup  -  Called by clock interrupt handler to awaken processes
 *------------------------------------------------------------------------
 */
void	wakeup(void)
{
	/* Awaken all processes that have no more time to sleep */

	resched_cntl(DEFER_START);
	struct qnewentry *head,*tail;
	head=getsleephead();
	tail=getsleeptail();
	while(head->qnext!=tail && head->qnext->qkey<=0)
	{
		ready(dequeue(sleepq));
	}
	resched_cntl(DEFER_STOP);
	return;
}
