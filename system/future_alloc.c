#include<xinu.h>
#include<future.h>

/*Allocates memory.
 Returns:
    structure future * if successful
    Null if memory is not allocated or future_flag is not FUTURE_EXCLUSIVE
 */
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
