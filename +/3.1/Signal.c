#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// SIGPIPE SIGTERM SIGUSR1



unsigned char symb;
int fl;

void readerPipe(int s)
{
	//printf("readerPipe\n");
	fl = 2;
}

void readerChl(int s)
{
	//printf("readerChl\n");
	//printf("child is dead\n");
	exit(0);
}

void writerTerm(int s)
{
	//printf("writerTerm\n");
	return;
}

void readerUsr1(int s)
{
	//printf("readerUsr1\n");
	symb <<= 1;
}

void readerUsr2(int s)
{
	//printf("readerUsr2\n");
	
	symb <<= 1;
	symb += 1;
	//printf("USE\n");
	
}

void readerTerm(int s)
{
	//printf("readerTerm\n");
	fl = 1;
}

void sendword(unsigned char word, int pid)
{
	//printf("sendword %d\n", word);
	int i = 0;
	unsigned char bit = 1;
	bit <<= 7;


	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask, SIGTERM);


	for (i = 0; i < 8; ++i)
	{
		if ((bit & word) == 0)
			kill(pid, SIGUSR1);
		else
			kill(pid, SIGUSR2);
		bit >>= 1;
		////printf("%d\n", bit);
		sigsuspend(&mask);
		
	}
	kill(pid, SIGTERM);
	sigsuspend(&mask);
	
}



int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		//printf("ERROR\n");
		return 0;
	}


	sigset_t standart_set;
	sigemptyset(&standart_set);
	sigaddset(&standart_set, SIGUSR2);
	sigaddset(&standart_set, SIGUSR1);
	sigaddset(&standart_set, SIGTERM);
	sigaddset(&standart_set, SIGPIPE);
	sigaddset(&standart_set, SIGCHLD);

	sigprocmask(SIG_SETMASK, &standart_set, NULL);


	int ppid = getpid();




	int pid = fork();

	if(pid == 0)
	{
		//printf("%d\n", getpid());
		prctl(PR_SET_PDEATHSIG, SIGKILL);


		int curppid = getppid(); 
		if(curppid != ppid)
			exit(EXIT_FAILURE);




		struct sigaction sig_start;
		sig_start.sa_handler = &writerTerm;
		sig_start.sa_flags = 0;
		sigfillset(&sig_start.sa_mask);
		sigaction(SIGTERM, &sig_start, NULL);


		int fdin = open(argv[1], 0666 | O_RDONLY);
		if (fdin == -1)
			exit(EXIT_FAILURE);



		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIGTERM);
		

		while(1)
		{
			unsigned char buf[100];
			int res = read(fdin, buf, 100);


			if(res == -1)
				exit(EXIT_FAILURE);
			
			int i = 0;

			for (i = 0; i < res; ++i)
			{
				sendword(buf[i], ppid);
				//printf("%d\n", i);
				
			}

			if (res < 100)
			{
				kill(ppid, SIGPIPE);
				sigsuspend(&mask);	
			}

		}



	}
	else
	{
		//printf("%d\n", getpid());
		symb = 0;

		struct sigaction sig_start;
		sig_start.sa_handler = &readerChl;
		sig_start.sa_flags = 0;
		sigfillset(&sig_start.sa_mask);
		sigaction(SIGCHLD, &sig_start, NULL);


		sig_start.sa_handler = &readerTerm;
		sig_start.sa_flags = 0;
		sigfillset(&sig_start.sa_mask);
		sigaction(SIGTERM, &sig_start, NULL);


		sig_start.sa_handler = &readerUsr1;
		sig_start.sa_flags = 0;
		sigfillset(&sig_start.sa_mask);
		sigaction(SIGUSR1, &sig_start, NULL);


		sig_start.sa_handler = &readerUsr2;
		sig_start.sa_flags = 0;
		sigfillset(&sig_start.sa_mask);
		sigaction(SIGUSR2, &sig_start, NULL);




		sig_start.sa_handler = &readerPipe;
		sig_start.sa_flags = 0;
		sigfillset(&sig_start.sa_mask);
		sigaction(SIGPIPE, &sig_start, NULL);



		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIGTERM);
		sigdelset(&mask, SIGUSR1);
		sigdelset(&mask, SIGUSR2);
		sigdelset(&mask, SIGCHLD);
		sigdelset(&mask, SIGPIPE);
		//sigdelset(&mask, SIGINT);

		unsigned char buf[100];
		int ind = 0;

		while(1)
		{
			//printf("**\n");
			sigsuspend(&mask);
			//printf("*7*\n");
			//perror("sigsuspend");
			if (fl == 1)
			{
				buf[ind] = symb;
				ind++;
				symb = 0;
				fl = 0;
				if(ind == 100)
				{
					write(1, buf, 100);
					ind = 0;
				}

			}
			else if (fl == 2)
			{
				write(1, buf, ind);
				////printf("%d\n", ind);
				exit(0);
			}
			kill(pid, SIGTERM);
		}



	}

	
	return 0;
}