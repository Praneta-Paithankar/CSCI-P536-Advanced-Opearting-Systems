#include<xinu.h>
#include<future.h>
/*Frees memory*/
syscall future_free(future* f)
{
    freemem((char *)f->value,sizeof(int));  //free integer memory
    return freemem((char *)f,sizeof(future));
}
