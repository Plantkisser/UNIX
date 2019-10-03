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

	int fdin = open(buf, O_RDONLY | O_NONBLOCK);
	fcntl(fdin, F_SETFL, O_RDONLY);	mkfifo("request.txt", 0600);

	int fdout = open("request.txt", O_WRONLY);



	printf("Open request\n");
	int res = write(fdout, buf, 10);
	
	sleep(5); //time out!!!!!

	char bufrec[100];
	memset(bufrec, 0, 100);


	if (read(fdin, bufrec, 10) == 0)
	{
		printf("Time out\n");
		close(fdin);
		close(fdout);
		remove(buf);
		return 0;
	}
	printf("%s", bufrec);
	memset(bufrec, 0, 100);	

	res = 1;
	while(res != 0)
	{
		res = read(fdin, bufrec, 10);


		printf("%s", bufrec);
		memset(bufrec, 0, 100);
	}

	close(fdin);
	remove(fdout);
	close(fdout);
	remove(buf);
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
