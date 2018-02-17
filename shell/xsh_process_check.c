#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<process_ring.h>
#include <clock.h>

void p1()
{
	
	printf("process1\n");
	sleep(1);
	printf("process1 after sleep\n");

}


void p2()
{
	printf("process2\n");
	sleep(1);
	printf("process2 after sleep\n");
}
shellcmd xsh_process_check(int argc, char *args[]) {
	
	pid32 pr1=create(p1,1024,24,"p1",0);
	pid32 pr2=create(p2,1024,25,"p2",0);
	printf("Main process sleep\n");
	resume(pr1);
	resume(pr2);
	sleep(2);
	return SHELL_OK;
}

