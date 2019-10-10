#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>


struct msgbuf
{
	long mtype;
	char mtext[10];
};


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Wrong argument\n");
		return 0;
	}


	int qid = msgget(IPC_PRIVATE, 0666);
	if (qid == -1)
		return 0;

	int n = strtol(argv[1], NULL, 10);

	int i = 0;
	int pid = 0;
	for(i = 1; i <= n; i++)
	{
		pid = fork();
		if (pid == 0)
			break;
		if (pid == -1)
		{
			msgctl(qid, RMID, NULL);
			printf("Too big number of processes\n");
		}
	}
	i--;

	if (pid != 0)
	{
		printf("%d\n", i);


		struct msgbuf msg;
		msg.mtype = 1;
		msgsnd(qid, &msg, 10, 0);

	
	}
	else
	{
		struct msgbuf msg;
		if (msgrcv(qid, &msg, 10, i, 0) == -1)
		{
			//printf("ERROR %d\n", i);
			return 0;
		}
		printf("%d\n", i);
			
		msgrcv(qid, &msg, 10, n + 1, 0);

		i++;
		if (i <= n)
		{
			struct msgbuf msg_next;
			msg_next.mtype = i;
			msgsnd(qid, &msg_next, 10, 0);
		}

	}

	return 0;

}