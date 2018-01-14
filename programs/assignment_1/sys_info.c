#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<stdlib.h>
#include <string.h>
#include <sys/wait.h> 
int  main(int argc,char*argv[]){
	pid_t pid;
	int fd[2];
	char * buffer;
	int count=0;
	char * command=NULL;
	if(argc !=2)
	{	
		fprintf(stderr,"Usage %s <path to another program>.",argv[0]);
		exit(1);
	}
	if(pipe(fd)==-1) //Create Pipe
	{
		perror("Pipe");
		exit(1);
	}
	pid=fork();	//Fork the process
	if(pid==-1)
	{
		perror("Fork");
		exit(1);
	}
	if(pid >0) //parent process
	{
		printf("\nParent Pid =  %d.",getpid());
                close(fd[0]);   //close read  descriptor
                count=write(fd[1],&argv[1],strlen(argv[1])+1);
                if(count==-1)
                {
                        perror("write");
                }
                close(fd[1]);
                wait(NULL);
		exit(0);
	}
	if(pid==0) //Child process
	{
		close(fd[1]);
		printf("\nChild Pid =  %d.",getpid());
		//close(fd[1]);	//Close write descriptor
		count=read(fd[0],&buffer,sizeof(buffer));
		if(count<=0)
		{
			perror("Read");
			exit(-1);
		}
		command=buffer+5;
		if (!strcmp(buffer,"/bin/echo"))
		{	execl(buffer,command,"Hello World!",(char*)NULL);
			perror("execl");
		}else
		{
			execl(buffer,command,(char *) NULL);
			perror("execl");
		}
		close(fd[0]);
		exit(0);
	}
	return 0;
}
