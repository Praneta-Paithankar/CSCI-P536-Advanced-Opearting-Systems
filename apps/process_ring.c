#include<xinu.h>
#include<stdio.h>
#include<process_ring.h>
/*Polling function */
process  polling(int32* currentProcess,int32* nextProcess,int32 currentRing,int32 currentRound)
{
	*nextProcess=*currentProcess-1;
	printf("Ring Element %d : Round %d : Value : %d\n",currentRing,currentRound,*currentProcess);	
	return OK;
}

process polling2(int32* currentProcess,int32* nextProcess,int32 roundCount,int32 currentRing)
{
	int32 currentRound=0;
	while(currentRound<roundCount)
	{
		if(*currentProcess!=-1)
		{
			printf("Ring Element %d : Round %d : Value : %d\n",currentRing,currentRound,*currentProcess);	
			currentRound+=1;
			*nextprocess=*currentProcess-1;
			
		}
	}
	return OK;
}
/*Synchronization using message passing*/
process message_passing(pid32 mainPid,pid32* nextProcessPid,int32 roundCount,int32 currentRing)
{
	int32 value;
	int32 currentRound=0;
	while(currentRound<roundCount)
	{
		value=receive(); //Receive current value from previous process
		printf("Ring Element %d : Round %d : Value : %d\n",currentRing,currentRound,value);
		currentRound++;
		send(*nextProcessPid,--value); //send new value to next process
	}
	send(mainPid,currentRing); //send message to main process
	return OK;
}
