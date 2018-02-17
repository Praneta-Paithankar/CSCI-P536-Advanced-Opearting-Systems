/* insertd.c - insertd */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insertd  -  Insert a process in delta list using delay as the key
 *------------------------------------------------------------------------
 */
status	insertd(			/* Assumes interrupts disabled	*/
                pid32		pid,		/* ID of process to insert	*/
                qid16		q,		/* ID of queue to use		*/
                int32		key		/* Delay from "now" (in ms.)	*/
)
{
    int32	next;			/* Runs through the delta list	*/
    int32	prev;			/* Follows next through the list*/
    if (isbadpid(pid)) {
        return SYSERR;
    }
    if(q==sleepq)
    {
        struct qnewentry *head=getsleephead();
        struct qnewentry *snext,*tail,*n;
        n=(struct qnewentry *)getmem(sizeof(struct qnewentry));
        n->process_id=pid;
        snext= head->qnext;
        tail=getsleeptail();
        
        while(snext!=tail && snext->qkey <= key)
        {
            key=key-snext->qkey;
            snext=snext->qnext;
        }
        
        n->qkey=key;
        n->qnext=snext;
        n->qprev=snext->qprev;
        snext->qprev->qnext=n;
        snext->qprev=n;
        if (snext != tail)
        {
            snext->qkey -= key;
        }
        return OK;
    }
    if (isbadqid(q)) {
        return SYSERR;
    }
    prev = queuehead(q);
    next = queuetab[queuehead(q)].qnext;
    while ((next != queuetail(q)) && (queuetab[next].qkey <= key)) {
        key -= queuetab[next].qkey;
        prev = next;
        next = queuetab[next].qnext;
    }
    
    /* Insert new node between prev and next nodes */
    
    queuetab[pid].qnext = next;
    queuetab[pid].qprev = prev;
    queuetab[pid].qkey = key;
    queuetab[prev].qnext = pid;
    queuetab[next].qprev = pid;
    if (next != queuetail(q)) {
        queuetab[next].qkey -= key;
    }
    
    return OK;
}
