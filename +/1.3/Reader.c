#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>




void translate(int num, char* buf);


int main()
{
	again:
	mkfifo("request.txt", 0600);

	int cpid = getpid();
	char buf[10] = "0000000000";

	translate(cpid, buf);

	mkfifo(buf, 0600);


	//int pid = fork();
	

	// begin




	int fdin = open(buf, O_RDONLY | O_NONBLOCK);
	// fcntl
	int fdout = open("request.txt", O_WRONLY);
	fcntl(fdout, F_SETFL, O_WRONLY | O_NONBLOCK);






	printf("Open request\n");
	int res = write(fdout, buf, 10);
	/*if(res == -1 && errno == EPIPE)
	{
		close(fdin);
		close(fdout);
		printf("**\n");
		goto again;
	}*/
	
		sleep(5); //time out!!!!!

	char bufrec[100];
	memset(bufrec, 0, 100);


	if (read(fdin, bufrec, 10) == 0)
	{
		printf("Time out\n");
		//sleep(10);
		close(fdin);
		close(fdout);
		remove(buf);
		return 0;
	}
	printf("%s", bufrec);
	memset(bufrec, 0, 100);	

	fcntl(fdin, F_SETFL, O_RDONLY);

	res = 1;
	while(res != 0)
	{
		res = read(fdin, bufrec, 10);


		printf("%s", bufrec);
		memset(bufrec, 0, 100);
	}

	close(fdin);
	close(fdout);
	remove(buf);
	//end
	return 0;

}

void translate(int num, char* buf)
{
	int i = 0;
	while(num > 0)
	{
		buf[i] = num % 10 + '0';
		i++;
		num /= 10;
	}
}


