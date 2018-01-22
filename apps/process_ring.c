#include<xinu.h>
#include<stdio.h>
#include<process_ring.h>
process  polling(int32* currentProcess,int32* nextProcess,int32 currentRing,int32 currentRound)
{
	*nextProcess=*currentProcess-1;
	printf("Ring Element %d : Round %d : Value : %d\n",currentRing,currentRound,*currentProcess);	
	return OK;
}
process message_passing(pid32 mainPid,pid32* nextProcessPid,int32 roundCount,int32 currentRing)
{
	int32 value;
	int32 currentRound=0;
	while(currentRound<roundCount)
	{
		value=receive();
		printf("Ring Element %d : Round %d : Value : %d\n",currentRing,currentRound,value);
		currentRound++;
		send(*nextProcessPid,--value);
	}
	//printf("Ring element %d %d",currentRing,mainPid);
	send(mainPid,currentRing);
	//printf("Return element %d",currentRing);
	return OK;
}
