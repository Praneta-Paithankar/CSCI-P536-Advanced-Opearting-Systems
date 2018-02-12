#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<process_ring.h>
#include <clock.h>

void p1()
{

	printf("process1\n");
	resched2(PR_CURR);
	printf("process1 after sleep\n");
}

void p2(pid32 p1)
{
//	for(int i=0;i<15;i++)
	printf("process2\n");
	chprio(p1,20);
	resched2(PR_READY);
	printf("After resched2 process2\n");
}

void p3()
{
	printf("process3\n");
	resched2(PR_CURR);
	printf("After resched2 process3\n");
}
shellcmd xsh_resched_check(int argc, char *args[]) {
	
	pid32 pr1=create(p1,1024,20,"p1",0);
	//printf("main process\n");
	pid32 pr2=create(p2,1024,21,"p2",1,pr1);
	pid32 pr3=create(p3,1024,25,"p3",0);
	resume(pr2);
	printf("main process\n");
	resume(pr1);
	resume(pr3);
	return SHELL_OK;
}

