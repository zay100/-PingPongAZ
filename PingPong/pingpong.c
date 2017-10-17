#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

int fd[2];
int stop;
pid_t next_pid,my_pid,son1_pid,son2_pid,parent_pid;
void SigHndlr(int s);
void TarasBulba(int s);


int main(int argc, char *argv[])
{
	pid_t tmp_pid;
	int status,ball=0;
	
	
	signal(SIGUSR1,SigHndlr);
    signal(SIGUSR2,TarasBulba);
	
	if (argc<2)
	{
		printf("not enough arg");
		return 1;
	}
	
	stop=atoi(argv[1]);
    printf("stop %d\n",stop);
	pipe(fd);
	
 	parent_pid=tmp_pid=getpid();
	printf("\n Father PID=%d \n", tmp_pid);
	
	if (stop==0)
	{
		printf("Father PID=%d \n", tmp_pid);
		return 1;
	}

	tmp_pid=fork();
	if (tmp_pid==-1)
	{
		printf("\n error creating son1\n");
		return 1;
	}
	
	if (!tmp_pid)
	{		//we are in son1
		next_pid=getppid();
        my_pid=getpid();
		printf("\n I'm Son 1  PID=%d \n", my_pid);
		for(;;);
	}
	
	else
	{		//we are in father with one son
		next_pid=son1_pid=tmp_pid;  //второй сын будет отправлять мяч первому сыну
		// printf("Try to born 2-nd son. 1-st son PID=%d\n",tmp_pid);
		tmp_pid=fork();
	
		if (tmp_pid==-1)
		{
			printf("error");
			return 1;
		}
		
		if (!tmp_pid)
		{		//we are in son2
			my_pid=getpid();
            //next_pid=getppid();
            printf("\n I'm son 2. PID=%d \n", next_pid);
            for(;;);
          
		}
		else
		{		//we are in father with two sons
		
			next_pid=son2_pid=tmp_pid;
			my_pid=getpid();
			sleep (1);
            printf("\n I'm Father are ready to start. Next is=%d\n", next_pid);
			write(fd[1], &ball, sizeof(int));
            kill(next_pid,SIGUSR1);
            for(;;);
            
        }
	}
}

void SigHndlr(int s)
{
	signal(SIGUSR1,SigHndlr);
    int local_ball; // Локальный счетчик бросков "для честности"
	
    //sleep(1);
    
	//printf("\nProcess %d got the signal %d, the next is %d\n", my_pid, s, next_pid);
	if (read(fd[0],&local_ball,sizeof(int))<=0)
	{
		// посылается сигнал следующему по очереди процессу и закрывается текущий процесс
        printf("\nCan not read from pipe.  \n");
        kill(parent_pid,SIGUSR2);
		return;
	}
	printf("\nPID %d got the ball #%d\n", my_pid,local_ball);
	if (local_ball==stop)
	{
		printf("\nGame stopped at PID=%d  \n", my_pid);
		close(fd[1]);
        close(fd[0]);
        if (my_pid==parent_pid) TarasBulba(1);
        kill(parent_pid,SIGUSR2);
	}
	else
	{
		local_ball++;
		write(fd[1], &local_ball, sizeof(int));
        //printf("Send next\n");
        kill(next_pid,SIGUSR1);
	}
}

void TarasBulba(int s)
{
    //printf("\nProcess %d got the signal%d\n", my_pid, s);
    kill(son1_pid,SIGINT);
    kill(son2_pid,SIGINT);
    exit(0);
}

