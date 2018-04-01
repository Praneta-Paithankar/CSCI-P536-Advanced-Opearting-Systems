#include<xinu.h>
#include<future.h>
/*Frees memory*/
syscall future_free(future* f)
{
    freemem((char *)f->value,sizeof(int));  //free integer memory
    f_queue* head=f->set_queue;
    while (head!=NULL) {
        kill(head->process_id);
        freemem((char *)head,sizeof(f_queue));  //free set_queue
        head=head->qnext;
    }
    head=f->get_queue;
    while (head!=NULL) {
        kill(head->process_id); //Kill processes
        freemem((char *)head,sizeof(f_queue));  //free get_queue
        head=head->qnext;
    }
   
    return freemem((char *)f,sizeof(future));
}
