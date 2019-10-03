#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>




int main(int argc, char const *argv[])
{
	if(argc != 2)
	{
		printf("Error wrong arguments\n");
		return 0;
	}

	again:

	mkfifo("request.txt", 0600);
	int fdin = open("request.txt", O_RDONLY);



	char buf[11] = "0000000000";
	if (read(fdin, buf, 10) == 0 )
	{
		//printf("**\n");
		close(fdin);
		goto again;
	}


	int fdinnew = open(argv[1], O_RDONLY);
	assert(fdin != -1);

	//sleep(2);
	int fdout = open(buf, O_WRONLY | O_NONBLOCK);
	if (fdout == -1)
	{
		printf("Local pipe is dead\n");


		goto again;
	}
	fcntl(fdout, F_SETFL, O_WRONLY);

	char bufwrt[11];
	memset(bufwrt, 0, 11);
	int res = 0;
	int ret = 1;



	while((res = read(fdinnew, bufwrt, 10)) > 0)
	{
		write(fdout, bufwrt, 10);

		memset(bufwrt, 0, 11);

	}
	printf("%d\n", ret);


	close(fdin);
	close(fdinnew);
	close(fdout);
	return 0;

}