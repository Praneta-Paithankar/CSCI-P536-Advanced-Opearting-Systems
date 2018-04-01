#include<xinu.h>
#include<future.h>

extern void f_enqueue(f_queue** queue,pid32 pid);
extern pid32 f_dequeue(f_queue** queue);

/* Set the value of future by given value
  if future is null then return SYSERR
  if future state is empty then set the value.
  if future state is waiting then set the value and resume the process
  if future state is valid then return SYSERR
 */

syscall future_set(future *f, int *value)
{
    intmask    mask;/* Saved interrupt mask        */
    pid32 pid;
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
            *f->value=*value;
            f->state= FUTURE_VALID;
            restore(mask);
        }
        else if(f->state == FUTURE_WAITING)
        {
            *f->value=*value;
            f->state= FUTURE_VALID;
            
            resume(f->pid);   //resume process
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
            *f->value=*value;
            f->state= FUTURE_VALID;
            restore(mask);
        }
        else if(f->state == FUTURE_WAITING)
        {
            *f->value=*value;
            f->state= FUTURE_VALID;
            restore(mask);
            while(f->get_queue!=NULL){
                pid=f_dequeue(&(f->get_queue));
                resume(pid);//resume process
            }
        }
        else
        {
            restore(mask);
            return SYSERR;
        }
    }
    else if(f->flag == FUTURE_QUEUE)
    {
        if(f->get_queue!=NULL)
        {
            *f->value=*value;
            f->state= FUTURE_VALID;
            restore(mask);
            pid=f_dequeue(&(f->get_queue));
            resume(pid);//resume process
        }
        else
        {
            restore(mask);
            f->state = FUTURE_EMPTY;
            f_enqueue(&(f->set_queue),getpid());
            suspend(getpid());    //suspend process
            *f->value=*value;
        }
    }
    
    return OK;
}


