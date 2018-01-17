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
		fprintf(stderr,"Invalid number of arguments.\nCorrect usage: %s <path to another program>.\n",argv[0]);
		exit(1);
	}
	if(pipe(fd)==-1) //Create Pipe
	{
		perror("Pipe"); //if creation pipe fails
		exit(1);
	}
	pid=fork();	//Fork the process
	if(pid==-1)
	{
		perror("Fork"); //if creation of child process fails
		exit(1);
	}
	if(pid >0) //parent process
	{
		printf("Parent PID =  %d.\n",getpid());
                close(fd[0]);   //close read  descriptor
                count=write(fd[1],&argv[1],strlen(argv[1]));//Write program path
		if(count==-1)
                {
                        perror("write");
                }
                close(fd[1]); //close write descriptor
                wait(NULL);
	}
	else if(pid==0) //Child process
	{
		
		printf("Child PID =  %d.\n",getpid());
		close(fd[1]);	//Close write descriptor
		count=read(fd[0],&buffer,sizeof(buffer));//read program path
		if(count<=0)
		{
			perror("Read");
			exit(-1);
		}
		command=buffer+5;	//read program name from path e.g /bin/echo is path then echo is command
		if (!strcmp(buffer,"/bin/echo"))
		{	execl(buffer,command,"Hello World!",(char*)NULL);
			perror(command);
		}else
		{
			execl(buffer,command,(char *) NULL);
			perror("execl");
		}
		close(fd[0]);//close read descriptor
	}
	return 0;
}
