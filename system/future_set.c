#include<xinu.h>
#include<future.h>
/* Set the value of future by given value
  if future is null then return SYSERR
  if future state is empty then set the value.
  if future state is waiting then set the value and resume the process
  if future state is valid then return SYSERR
 */

syscall future_set(future *f, int *value)
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
        *f->value=*value;
        f->state= FUTURE_VALID;
    }
    else if(f->state == FUTURE_WAITING)
    {
        *f->value=*value;
        f->state= FUTURE_VALID;
        resume(f->pid);   //resume process
    }
    else
    {
        restore(mask);
        return SYSERR;
    }
    restore(mask);
    return OK;
}


