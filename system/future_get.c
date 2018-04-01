#include<xinu.h>
#include <future.h>

extern void f_enqueue(f_queue** queue,pid32 pid);
extern pid32 f_dequeue(f_queue** queue);

/* get the value from future.
   if future is menty then it returns SYSERR
   else it stores value of future in value variable.
 */
syscall future_get(future *f, int *value)
{
    intmask    mask;            /* Saved interrupt mask        */
    mask = disable();
    
    if(f==NULL)
    {
        restore(mask);
        return SYSERR;
    }
    if(f->flag == FUTURE_EXCLUSIVE)
    {
        if(f->state==FUTURE_EMPTY)      // future is empty
        {
            f->pid=getpid();
            f->state= FUTURE_WAITING;
            
            suspend(f->pid);    //suspend process
            *value = *f->value;
            restore(mask);
        }
        else if(f->state == FUTURE_VALID)  //valid state
        {
            *value = *f->value;
            f->state=FUTURE_EMPTY;
            restore(mask);
        }
        else
        {
            restore(mask);
            return SYSERR;
        }
    }
    else if(f->flag == FUTURE_SHARED)
    {
        
        if(f->state==FUTURE_EMPTY)      // future is empty
        {
            f->pid=getpid();
            f->state= FUTURE_WAITING;
            f_enqueue(&(f->get_queue),f->pid);
            restore(mask);
            suspend(f->pid);    //suspend process
            *value = *f->value;
        }
        else if(f->state ==FUTURE_WAITING)
        {
            f->pid=getpid();
            f_enqueue(&(f->get_queue),f->pid);
            restore(mask);
            suspend(f->pid);    //suspend process
            *value = *f->value;
        }
        else if(f->state == FUTURE_VALID)  //valid state
        {
            *value = *f->value;
            restore(mask);
        }
        else
        {
            restore(mask);
            return SYSERR;
        }
    }
    else if(f->flag == FUTURE_QUEUE)
    {
        if (f->set_queue != NULL) {
            restore(mask);
            pid32 p_id=f_dequeue(&(f->set_queue));
            resume(p_id);
            *value = *f->value;
            f->state=FUTURE_EMPTY;
        }
        else
        {
            f_enqueue(&(f->get_queue),getpid());
            restore(mask);
            f->state=FUTURE_WAITING;
            suspend(getpid()); //suspend process
            *value = *f->value;
        }
    }
    
    return OK;
}
