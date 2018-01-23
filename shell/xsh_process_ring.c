#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<process_ring.h>
#include <clock.h>
/*------------------------------------------------------------------------
 *  *  * xsh_process_ring - run processes for multpile round
 *   *   *------------------------------------------------------------------------
 *    *    */

void print_usage(void){
  printf("see -- help for more information \n");
}

shellcmd xsh_process_ring(int argc, char *args[]) {
	int32 processCount=2;
	int32 roundCount=3;
	int32 i;
	char * imple="poll";
	//if --help option is used.
	if(!strncmp(args[1],"--help",7))
    	{
		printf("Usage: %s\n\n", args[0]);
 		printf("Description:\n");
        	printf("\tExecutes process ring.\n");
        	printf("Options:\n");
        	printf("\t-r round_no\tnumber of rounds.It should be positive integer.Default value is 3\n");
        	printf("\t-p process_no\tnumber of processes.It should be between 1-64.Default value is 2\n");
        	printf("\t-i type\t implemeentation type.Implementation type shoule be 'poll' or 'sync'.Default value is poll.");
        	printf("\t round_no process_no \t specify number of rounds and number of processes.");
        	printf("\t--help\tdisplay this help.\n");
   		return SHELL_OK;
	}
	for(i=1;i<argc;i++)
	{
		if(!strncmp(args[i],"-p",3)) //number of processes
		{
			if(i+1>argc) //number is missing
			{
				printf("-p flag expected an argument\n");
				return SHELL_ERROR;	
			}
			processCount=atoi(args[i+1]);
			if (!(0 < processCount && processCount <= 64))
			{
                		/* The number is out of range */
				printf("-p flag expected a number between 0 - 64\n");
                		print_usage();
                		return SHELL_ERROR;
            		}
			/* Skip pass the numeric argument,that was successfully parsed */
			i += 1;
		}
		else if(!strncmp(args[i],"-r",3)) //number of rounds
        	{
            		if(i+1>argc) //number is missing
			{
                		printf("-r flag expected an argument\n");
                		print_usage();
                		return SHELL_ERROR;
            		}
           		roundCount=atoi(args[i+1]);
            		if (roundCount<=0)
            		{
                		/* The number is negative*/
                		printf("-r flag expected positive number\n");
                		print_usage();
                		return SHELL_ERROR;
            		}
            		/* Skip pass the numeric argument,that was successfully parsed */
            		i += 1;
        	}
		else if(!strncmp(args[i],"-i",3)) //type of implementation 
		{
			if(i+1>argc) //type is missing
			{
				printf("-i flag expected an argument\n");
                		print_usage();
                		return SHELL_ERROR;
			}
			if((!strncmp(args[i+1],"poll",5))||(!strncmp(args[i+1],"sync",5) ))
			{
				imple=args[i+1];
			}
			else	
			{
				print_usage();
		                printf("-i flag expected argument as 'poll' or 'sync'\n");
        		        print_usage();
                		return SHELL_ERROR;
			}
			i=i+1;
		}
		else if(argc==3) //progress_ring round_no process_no
		{
			roundCount=atoi(args[1]);
           		if (roundCount<=0)
            		{
                		/* The number is out of range */
              		 	printf("-r flag expected positive number\n");
               			print_usage();
                		return SHELL_ERROR;
            		}
            		processCount=atoi(args[i+1]);
			if(processCount==0)
			{
				/* No number was parsed.*/
                		printf("-p flag expected an integer\n");
                		print_usage();
                		return SHELL_ERROR;
			}
			else if (!(0 <= processCount && processCount <= 64)) 
			{
        		        /* The number is out of range */
                		printf("-p flag expected a number between 0 - 64\n");
                		print_usage();
                		return SHELL_ERROR;
            		}
			break;
		}
        	else //invalid options
        	{
			printf("Invalid options.\n");
			print_usage();
            		return SHELL_ERROR;
        	}
		
	}
	printf("Number of Processes: %d\n",processCount);
	printf("Number of Rounds: %d\n",roundCount);
	int32 totalCount=processCount*roundCount-1;
	int32 processArray[processCount];
	int32 startTime;
	if(!strncmp(imple,"poll",5)) //polling method
	{
        	int32 rincrement=0;
        	int32 pincrement=0;
       		int32 currentIndex=0;
        	startTime=clktime; 
		for(i=0;i<processCount;i++)
			processArray[i]=-1;
	        processArray[0]=totalCount;
		while(totalCount>=0)
		{
			currentIndex=pincrement%processCount;	
			resume(create(polling,1024,30,"polling",4,&processArray[currentIndex],&processArray[(pincrement+1)%processCount],currentIndex,rincrement));
			if(((pincrement+1)%processCount)== 0)
				rincrement+=1;
			pincrement+=1;
			totalCount-=1;
		}
		printf("Elapsed Seconds: %d\n",clktime-startTime);
	}
	else if(!strncmp(imple,"sync",5)) //synchronization
	{
        	startTime=clktime;
		pid32 main_pid=getpid();
		for(i=0;i<processCount;i++)
		{
			processArray[i]=create(message_passing,1024,30,"message_passing",4,main_pid,&processArray[(i+1)%processCount],roundCount,i);
		}
		send(processArray[0],totalCount);
		for(i=0;i<processCount;i++)
		{
			resume(processArray[i]); //start execution of processes
		}
		receive();
		printf("Elapsed Seconds: %d\n",clktime-startTime);
	}
	return SHELL_OK;
}
