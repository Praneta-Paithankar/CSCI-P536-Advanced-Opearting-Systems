#include<xinu.h>
#include<future.h>


future* future_alloc(int future_flag)
{
    future* future_node=(future *) getmem(sizeof(future));
    if((int32)future_node ==SYSERR || future_flag != FUTURE_EXCLUSIVE)  //wrong flag or memory allocation fails
    {
        return NULL;
    }
    future_node->value=(int *)getmem(sizeof(int));
    future_node->flag=future_flag;
    future_node->state=FUTURE_EMPTY;
    return future_node;
}

syscall future_free(future* f)
{
    freemem((char *)f->value,sizeof(int));  //free integer memory
    return freemem((char *)f,sizeof(future));
}

syscall future_get(future *f, int *value)
{
    intmask    mask;            /* Saved interrupt mask        */
    mask = disable();
    if(f->state==FUTURE_EMPTY)      // future is empty
    {
        f->pid=getpid();
        f->state= FUTURE_WAITING;
        suspend(f->pid);    //suspend process
        *value = *(f->value);
    }
    else if(f->state == FUTURE_VALID)  //valid state
    {
        *value = *(f->value);
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


syscall future_set(future *f, int *value)
{
    intmask    mask;            /* Saved interrupt mask        */
    mask = disable();
    if(f->state==FUTURE_EMPTY)      // future is empty
    {
        *f->value=*value;
        //kprintf("val:%d f:%d\n",*value,*(f->value));
        f->state= FUTURE_VALID;
    }
    else if(f->state == FUTURE_WAITING)
    {
        *f->value=*value;
        //kprintf("val:%d f:%d\n",*value,*(f->value));
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

