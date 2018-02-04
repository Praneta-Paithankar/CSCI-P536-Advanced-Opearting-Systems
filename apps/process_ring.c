#include<xinu.h>
#include<stdio.h>
#include<process_ring.h>
/*Polling function */
process  polling(int32 currentRing,int32* totalProcess,int32* totalRound,int32* inbox,volatile int *currentElement)
{
 
    int32 currentRound=0;
    int32 prevVal=-1;
    int32 processCount= *totalProcess;
    int32 roundCount = *totalRound;
    while(currentRound< roundCount)
    {
        int32 value=*currentElement;
        if(prevVal != value)
        {
            printf("Ring Element %d : Round %d : Value : %d \n",currentRing,currentRound,value);
            inbox[(currentRing+1)% processCount]=value-1;
            currentRound++;
            prevVal=value;
	    if(value == 0)
	    {
		inbox[currentRing]=-5;
	    }
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
