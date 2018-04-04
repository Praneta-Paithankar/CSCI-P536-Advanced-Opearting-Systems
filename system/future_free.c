#include<xinu.h>
#include<future.h>
/*Frees memory*/
syscall future_free(future* f)
{
    freemem((char *)f->value,sizeof(int));  //free integer memory
    f_queue* head=f->set_queue;
    while (head!=NULL) {
        kill(head->process_id);
        f_queue* temp=head->qnext;
        freemem((char *)head,sizeof(f_queue));  //free set_queue
        head=temp;
    }
    head=f->get_queue;
    while (head!=NULL) {
        kill(head->process_id); //Kill processes
        f_queue* temp=head->qnext;
        freemem((char *)head,sizeof(f_queue));  //free get_queue
        head=temp;
    }
   
    return freemem((char *)f,sizeof(future));
}
