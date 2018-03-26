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
    if(f->flag == FUTURE_EXCLUSIVE)
    {
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
    }
    else if(f->flag == FUTURE_SHARED)
    {
        if(f->state==FUTURE_EMPTY)      // future is empty
        {
            *f->value=*value;
            f->state= FUTURE_VALID;
        }
        else if(f->state == FUTURE_WAITING)
        {
            *f->value=*value;
            f->state= FUTURE_VALID;
            f_queue* head= f->get_queue;
            while(head!=NULL){
                resume(head->process_id);//resume process
                head=head->qnext;
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
            //f->state= FUTURE_VALID;
            resume(f->get_queue->process_id);   //resume process
            f_queue* temp =f->get_queue;
            f->get_queue=f->get_queue->qnext;
            freemem((char* )temp,sizeof(f_queue));
        }
        else
        {
            if(f->set_queue==NULL)
            {
                f->set_queue=(f_queue *) getmem(sizeof(f_queue));
                f->set_queue->process_id = getpid();
                f->set_queue->qnext = NULL;
                suspend(getpid());    //suspend process
                *f->value=*value;
            }
            else
            {
                f_queue* head= f->set_queue;
                while (head->qnext!=NULL)
                {
                    head=head->qnext;
                }
                head->qnext=(f_queue *) getmem(sizeof(f_queue));
                head->qnext->process_id=getpid();
                head->qnext->qnext=NULL;
                suspend(getpid());    //suspend process
                *f->value=*value;
            }
        }
    }
    restore(mask);
    return OK;
}


