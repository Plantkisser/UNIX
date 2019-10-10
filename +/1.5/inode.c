#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int CheckFID(int first, int second)
{
	struct stat firstStr, secondStr;
	fstat(first, &firstStr);
	fstat(second, &secondStr);

	if (firstStr.st_ino == secondStr.st_ino && firstStr.st_dev == secondStr.st_dev)
		return 1;
	return 0;
}

int main()
{
	int fd1 = open("1.txt", O_WRONLY);
	int fd2 = open("2.txt", O_RDONLY);

	printf("%d\n", CheckFID(fd1, fd2));
	return 0;
}