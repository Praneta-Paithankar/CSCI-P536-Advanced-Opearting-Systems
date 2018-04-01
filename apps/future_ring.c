#include <xinu.h>
#include <future.h>

uint future_ring(future *in, future *out) {
    int val;
    future_get(in, &val);
    printf("Process %d gets %d, puts %d\n", getpid(), val, val-1);
    val--;
    future_free(in);
    future_set(out, &val);
    return OK;
}

