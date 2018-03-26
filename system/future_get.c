#include<xinu.h>
#include <future.h>

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
    }
    else if(f->flag == FUTURE_SHARED)
    {
        if(f->state==FUTURE_EMPTY)      // future is empty
        {
            f->pid=getpid();
            f->state= FUTURE_WAITING;
            if(f->get_queue==NULL){
                f->get_queue=(f_queue *) getmem(sizeof(f_queue));
            }
            f->get_queue->process_id = f->pid;
            f->get_queue->qnext = NULL;
            suspend(f->pid);    //suspend process
            *value = *f->value;
        }
        else if(f->state ==FUTURE_WAITING)
        {
            f->pid=getpid();
            f_queue *temp = f->get_queue->qnext;
            f->get_queue->qnext = (f_queue*)getmem(sizeof(f_queue));
            f->get_queue->qnext->process_id = f->pid;
            f->get_queue->qnext->qnext = temp;
            suspend(f->pid);    //suspend process
            *value = *f->value;
        }
        else if(f->state == FUTURE_VALID)  //valid state
        {
            *value = *f->value;
//            f->state=FUTURE_EMPTY;
        }
        else
        {
            restore(mask);
            return SYSERR;
        }
    }
    else if(f->flag == FUTURE_QUEUE)
    {
        if (f->set_queue !=NULL) {
            resume(f->set_queue->process_id);
            f_queue* temp =f->set_queue;
            f->set_queue=f->set_queue->qnext;
            freemem((char* )temp,sizeof(f_queue));
            *value = *f->value;
        }
        else
        {
            if(f->get_queue==NULL)
            {
                f->get_queue=(f_queue *) getmem(sizeof(f_queue));
                f->get_queue->process_id = getpid();
                f->get_queue->qnext = NULL;
                suspend(getpid());    //suspend process
                *value = *f->value;
            }
            else
            {
                f_queue* head= f->get_queue;
                while (head->qnext!=NULL)
                {
                    head=head->qnext;
                }
                head->qnext=(f_queue *) getmem(sizeof(f_queue));
                head->qnext->process_id=getpid();
                head->qnext->qnext=NULL;
                suspend(getpid());    //suspend process
                *value = *f->value;
            }
        }
    }
    restore(mask);
    return OK;
}
