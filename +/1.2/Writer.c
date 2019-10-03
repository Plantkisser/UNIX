#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>




int main(int argc, char const *argv[])
{
	sleep(7);
	if(argc != 2)
	{
		printf("Error wrong arguments\n");
		return 0;
	}

	mkfifo("request.txt", 0600);
	int fdin = open("request.txt", O_RDONLY);


	char buf[10] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
	while(read(fdin, buf, 10) == 0)
		sleep(2);

	//printf("%s\n", buf);

	int pid = fork();
	if (pid == 0)
		execl("/home/kihu/+/Real_writer", argv[1], buf, NULL);



	int wstatus;

	wait(&wstatus);
	close(fdin);
	return 0;

}