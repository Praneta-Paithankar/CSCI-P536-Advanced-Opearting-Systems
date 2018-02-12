#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<process_ring.h>
#include <clock.h>

void p1()
{

	printf("process1\n");
	receive();
	printf("process1 after message\n");
}

void p2()
{
	printf("process2\n");
	resched2(PR_READY);
	printf("After resched2 process2\n");
}

void p3(pid32 pr1)
{
	printf("process3\n");
	send(pr1,123);
	resched2(PR_CURR);
	printf("After resched2 process3\n");
}
shellcmd xsh_resched_check(int argc, char *args[]) {
	
	pid32 pr1=create(p1,1024,24,"p1",0);
	pid32 pr2=create(p2,1024,20,"p2",0);
	pid32 pr3=create(p3,1024,25,"p3",1,pr1);
	resume(pr1);
	resume(pr2);
	resume(pr3);
	return SHELL_OK;
}

