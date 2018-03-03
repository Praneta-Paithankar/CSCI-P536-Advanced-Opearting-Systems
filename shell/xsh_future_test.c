#include <xinu.h>
#include <future.h>

uint32 future_ring(future *in, future *out);

/*run command to check future*/
shellcmd xsh_future_test(int nargs,char * args[])
{
    int future_flags = 0;
    int ring_count = 10;
    int final_val;
    int i;
#ifndef _FUTURE_H_
    printf("no support for futures (_FUTURE_H_ not defined.)\n");
    return OK;
#endif
    if (nargs == 3 && (strncmp(args[1], "future_test",12) == 0) &&(strncmp(args[2], "-r", 3) == 0))
    {
        printf("Producer/consumer process ring\n");
        future *first, *in, *out = NULL;
        first = future_alloc(future_flags);
        in = first;
        for (i=0; i < ring_count; i++) {
            out = future_alloc(future_flags);
            resume( create(future_ring, 1024, 20, "", 2, in, out) );
            in = out;
        }
        printf("master sets %d\n", ring_count);
        future_set(first, &ring_count);
        future_get(out, &final_val);
        printf("master gets %d\n", final_val);
    }
    else
    {
        printf("Invalid option");
        return 1;
    }
    return 0;
}

uint future_ring(future *in, future *out) {
    int val;
    future_get(in, &val);
    printf("Process %d gets %d, puts %d\n", getpid(), val, val-1);
    val--;
    future_free(in);
    future_set(out, &val);
    return OK;
}


