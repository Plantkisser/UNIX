#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

enum contant
{
	READY = 10
};


int main()
{

	mkfifo("/home/kihu/+/text", 0600);
	mkfifo("/home/kihu/+/ready_for_reading", 0600);


	printf("*\n");
	int fready = open("/home/kihu/+/ready_for_reading", O_RDONLY);
	int fdin = open("/home/kihu/+/text", O_RDONLY);
	printf("*\n");

	char a = 0;

	while(1)
	{
		read(fready, &a, 1);
		
		if (a != READY)
		{
			printf("***, %d\n", a);
			sleep(5);
		}
		else
			break;
	}

	char buf[100];
	memset(buf, 0, 100);
	while(read(fdin, buf, 100) > 0)
	{
		printf("%s", buf);

		memset(buf, 0, 100);
	}

	close(fdin);
	close(fready);


	return 0;
}