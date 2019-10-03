#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>




void translate(int num, char* buf);

int main()
{
	mkfifo("request.txt", 0600);

	int cpid = getpid();
	char buf[10] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};

	translate(cpid, buf);

	mkfifo(buf, 0600);


	printf("%s\n", buf);

	int pid = fork();
	if(pid == 0)
		execl("/home/kihu/+/Real_reader", buf, NULL);

	int wstatus;

	wait(&wstatus);
	//remove("request.txt");
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