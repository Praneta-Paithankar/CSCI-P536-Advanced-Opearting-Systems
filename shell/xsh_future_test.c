#include <xinu.h>
#include <future.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int count=1;
int zero=0;
int one=1;
extern uint32 future_ring(future *in, future *out);
uint future_prod(future *fut);
uint future_cons(future *fut);
int ffib(int n);

future ** fibfut;
int** data_array;


/*run command to check future*/
shellcmd xsh_future_test(int nargs,char * args[])
{
    int future_flags = FUTURE_EXCLUSIVE;
    int ring_count = 10;
    int final_val;
    int i;
#ifndef _FUTURE_H_
    printf("no support for futures (_FUTURE_H_ not defined.)\n");
    return OK;
#endif
    if (nargs == 3 && (strncmp(args[1], "future_test",12) == 0))
    {
        if (strncmp(args[2], "-r", 3) == 0)
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
        else if(strncmp(args[2], "-pc", 4) == 0)
        {
            future *f_exclusive, *f_shared, *f_queue;
            count=1;
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
        }
        else
        {
            printf("No valid options\n");
            return 1;
        }
    }
    else if (nargs == 4 && (strncmp(args[1], "future_test",12) == 0) && strncmp(args[2], "-f", 3) == 0)
    {
        int N = atoi(args[3]);
        if (N>=1) {
            int i;
            fibfut = (future **) getmem(N*sizeof(future *));
            data_array= (int **)getmem(N*sizeof(int*));
            for (i=0; i<=N; i++) {
                fibfut[i] = future_alloc(FUTURE_SHARED);
                data_array[i] = (int *) getmem(sizeof(int));
            }
            future_set(fibfut[0],&zero);
            
            for (i=1; i<=N; i++) {
                resume(create(ffib,1024,20,"fib",1,i));

            }
            future_get(fibfut[N],data_array[N]);
            printf("Futures Fibonacci for N = %d\n",N);
            printf("Nth Fibonacci value for N= %d is %d \n",N,*data_array[N]);
            for (i=0; i<=N; i++) {
                future_free(fibfut[i]);
                freemem((char *)data_array[i],sizeof(int));
            }
            freemem((char*)data_array,N*sizeof(int*));
            freemem((char*)fibfut,N*sizeof(future *));
        }
       
    }
    else
    {
        printf("No valid options\n");
        return 1;
    }
    return 0;
}
uint future_prod(future *fut) {
    int j;
    j = count;
    count++;
    printf("Produced %d\n",j);
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
    printf("Consumed %d\n", i);
    return OK;
}
int ffib(int n) {
    
    int minus1 = 0;
    int minus2 = 0;
    int this = 0;
    
    if (n == 0) {
        future_set(fibfut[0], &zero);
        return OK;
    }
    
    if (n == 1) {
        future_set(fibfut[1], &one);
        return OK;
    }
    
    future_get(fibfut[n-2], &minus2);
    future_get(fibfut[n-1], &minus1);
    this = minus1 + minus2;
    
    future_set(fibfut[n], &this);
    
    return(0);
    
}




