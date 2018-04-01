#include <xinu.h>
#include <future.h>

extern uint future_prod(future *fut);

extern uint future_cons(future *fut);


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

