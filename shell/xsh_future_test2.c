#include <xinu.h>
#include <future.h>

uint future_prod(future *fut) {
    int i, j;
    //static int count=0;
    j = (int)fut;
    for (i=0; i<1000; i++) {
        j += i;
    }
//    j=count;
//    count++;
    future_set(fut, &j);
    return OK;
}

uint future_cons(future *fut) {
    
    int i, status;
    status = future_get(fut, &i);
    if (status < 1) {
        printf("future_get failed\n");
        return -1;
    }
    printf("it produced %d\n", i);
    return OK;
}

/*run command to check future*/
shellcmd xsh_future_test2(int nargs,char * args[])
{
    #ifndef _FUTURE_H_
        printf("no support for futures (_FUTURE_H_ not defined.)\n");
        return OK;
    #endif

    future *f_exclusive, *f_shared, *f_queue;
        
    f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
    f_shared = future_alloc(FUTURE_SHARED);
    f_queue = future_alloc(FUTURE_QUEUE);
    
    // Test FUTURE_EXCLUSIVE
    resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
    resume( create(future_prod, 1024, 20, "fprod1", 1, f_exclusive) );

   // Test FUTURE_SHARED
    resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
    resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
    resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) );
    resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
    resume( create(future_prod, 1024, 20, "fprod2", 1, f_shared) );

    
    // Test FUTURE_QUEUE
    resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod3", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod4", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod5", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod6", 1, f_queue) );
    
    sleep(2);
    return 0;
}
void test_future()
{
   
}
