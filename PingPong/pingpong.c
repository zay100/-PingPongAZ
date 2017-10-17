#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

int fd[2];
int stop;
pid_t next_pid,my_pid;
void SigHndlr(int s);


int main(int argc, char *argv[])
{
	pid_t tmp_pid,son1_pid,son2_pid;
	int ball=0;
	
	
	signal(SIGUSR1,SigHndlr);
	
	if (argc<2)
	{
		printf("not enough arg");
		return 1;
	}
	
	stop=atoi(argv[1]);
	pipe(fd);
	
 	tmp_pid=getpid();
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
		my_pid=getpid();
		printf("\n Son 1 born.  PID=%d \n", my_pid);
		next_pid=getppid();
		while (1);
		
	}
	
	else
	{		//we are in father with one son
		next_pid=tmp_pid;  //второй сын будет отправлять мяч первому сыну
		printf("Try to born 2-nd son. 1-st sin PID=%d\n",tmp_pid);
		tmp_pid=fork();
	
		if (tmp_pid==-1)
		{
			printf("error");
			return 1;
		}
		
		if (!tmp_pid)
		{		//we are in son2
			my_pid=getpid();
			printf("\n Son 2 born. PID=%d \n", my_pid);
			while (1);
		}
		else
		{		//we are in father with two sons
		
			next_pid=tmp_pid;
			my_pid=getpid();
			printf("\n I'm ready to start=%d Next is=%d\n", my_pid,next_pid);
			write(fd[1], &ball, sizeof(int));
			kill(next_pid,SIGUSR1);
			while (1);
		}
	}
}

void SigHndlr(int s)
{
	int local_ball; // Локальный счетчик бросков "для честности"
	
	printf("\nProcess %d got the signal, the next is %d\n", my_pid,next_pid);
	if (read(fd[0],&local_ball,sizeof(int))<=0)
	{
		// посылается сигнал следующему по очереди процессу и закрывается текущий процесс
			printf("\nClosing PID=%d  \n", my_pid);
		kill(next_pid,SIGUSR1);
		exit(0);
	}
	printf("\nPID %d got the ball #%d\n", my_pid,local_ball);
	if (local_ball==stop)
	{
		printf("\nGame stopped at PID=%d  \n", my_pid);
		close(fd[1]);
	}
	else
	{
		local_ball+=1;
		write(fd[1], &local_ball, sizeof(int));
	}
	// здесь послать сигнал действующему процессу
	kill(next_pid,SIGUSR1);
	
}

