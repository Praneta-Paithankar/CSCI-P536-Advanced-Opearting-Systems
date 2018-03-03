#include<xinu.h>
#include <future.h>

syscall future_get(future *f, int *value)
{
    intmask    mask;            /* Saved interrupt mask        */
    mask = disable();
    if(f==NULL)
    {
        restore(mask);
        return SYSERR;
    }
    if(f->state==FUTURE_EMPTY)      // future is empty
    {
        f->pid=getpid();
        f->state= FUTURE_WAITING;
        suspend(f->pid);    //suspend process
        *value = *f->value;
    }
    else if(f->state == FUTURE_VALID)  //valid state
    {
        *value = *f->value;
        f->state=FUTURE_EMPTY;
    }
    else
    {
        restore(mask);
        return SYSERR;
    }
    restore(mask);
    return OK;
}
