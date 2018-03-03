#include<xinu.h>
#include<future.h>

syscall future_free(future* f)
{
    freemem((char *)f->value,sizeof(int));  //free integer memory
    return freemem((char *)f,sizeof(future));
}
