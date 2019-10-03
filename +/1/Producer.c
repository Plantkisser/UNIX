#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
	if(argc != 2)
	{
		printf("Error wrong arguments\n");
		return 0;
	}

	int res = mkfifo("/home/kihu/+/request.txt", 0600);
	if(res != -1)
	{
		int pid = fork();
		if(pid == 0)
			execl("/home/kihu/+/manager", "/home/kihu/+/request.txt", NULL);
	}

	int fd = open("/home/kihu/+/request.txt", O_WRONLY);

	write(fd, argv[1], 20);
	printf("WRITE\n");
	
	return 0;

}

