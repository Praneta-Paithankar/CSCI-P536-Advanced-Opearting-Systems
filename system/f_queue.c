#include<xinu.h>
#include<f_queue.h>

//Enqueue process in queue
void f_enqueue(f_queue** queue,pid32 pid)
{
    if (*queue == NULL) {
        *queue= (f_queue* ) getmem (sizeof(f_queue));
        (*queue)->process_id = pid;
        (*queue)->qnext = NULL;
    }
    else
    {
        f_queue* head = *queue;
        while(head->qnext!=NULL)
        {
            head=head->qnext;
        }
        head->qnext = (f_queue*) getmem(sizeof(f_queue));
        head->qnext->process_id = pid;
        head->qnext->qnext = NULL;
    }
}
//Dequeue the process from queue
pid32 f_dequeue(f_queue** queue)
{
    if(*queue == NULL)
        return NULL;
    f_queue* head = *queue;
    pid32 pid=(*queue)->process_id;
    *queue=(*queue)->qnext;
    freemem((char*)head,sizeof(f_queue));
 	return pid;
}
