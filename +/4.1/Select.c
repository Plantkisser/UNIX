#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


int my_pow(int base, int k)
{
	int i = 0;
	int res = 1;

	if (k > 4)
		return 128;

	for (int i = 0; i < k; ++i)
	{
		res*= base;
	}

	return res;
}


struct BufferParent
{
	int end;
	char* data;
	int start;
	int capacity;
};

typedef struct BufferParent BufferParent;




int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("WRONG ARGUMENT\n");
		exit(EXIT_FAILURE);
	}

	int n = strtol(argv[1], NULL, 10);

	int i = 0;
	int maxr = 0;
	int maxw = 0;

	int *pipe_arr = (int*) calloc(n * 4, sizeof(int));
	for (i = 0; i < n * 4; i+= 2)
	{
		if (pipe(pipe_arr + i) == -1)
			exit(EXIT_FAILURE);
	}
	int pid = 0;
	for (i = 0; i < n; ++i)
	{
		pid = fork();
		if (pid == 0)
			break;
	}


	if (pid != 0)
	{
		int is_end = 0;
		unsigned long byte_count = 0;

		for (i = 0; i < 4*n; i+= 4)
		{
			fcntl(pipe_arr[i + 1], F_SETFL, O_WRONLY | O_NONBLOCK);
			fcntl(pipe_arr[i + 2], F_SETFL, O_RDONLY | O_NONBLOCK);
			close(pipe_arr[i]);
			close(pipe_arr[i+3]);
			maxr = maxr > pipe_arr[i + 2] ? maxr : pipe_arr[i + 2];
			maxw = maxw > pipe_arr[i + 1] ? maxw : pipe_arr[i + 1];
		}
		maxr++;
		maxw++;
		int max = maxr > maxw ? maxr : maxw;


		int fdin = open(argv[2], O_RDONLY);
		if(fdin == -1)
		{
			printf("ERROR open\n");
			exit(EXIT_FAILURE);
		}

		
		BufferParent *buf = (BufferParent*) calloc(n + 1, sizeof(BufferParent));

		for(i = 0; i < n + 1; ++i)
		{	
			buf[i].capacity = my_pow(3, n-i) * 1024;
			buf[i].data = (char*) calloc(my_pow(3, n-i) * 1024, sizeof(char));
		}


		while(1)
		{
			fd_set fd_read;
			fd_set fd_write;
			FD_ZERO(&fd_write);
			FD_ZERO(&fd_read);
			for (i = 0; i < n * 4; i+= 4)
			{
				if (buf[i/4 + 1].capacity - buf[i/4 + 1].end > 0)
					FD_SET(pipe_arr[i + 2], &fd_read);

				if (buf[i/4].start < buf[i/4].end)
					FD_SET(pipe_arr[i + 1], &fd_write);

			}
			if (!is_end && buf[0].capacity - buf[0].end > 0) FD_SET(fdin, &fd_read);
			
			if (buf[n].start < buf[n].end)
				FD_SET(1, &fd_write);


			if (select(max, &fd_read, &fd_write, NULL, NULL) == -1)
			{
				perror("Select");
				return 0;
			}

			if (FD_ISSET(fdin, &fd_read))
			{
				int count = 0;
				if ((count = read(fdin, buf[0].data + buf[0].end, buf[0].capacity - buf[0].end)) == -1)
				{	
					printf("read error\n");
					exit(EXIT_FAILURE);
				}
				
				buf[0].end+= count;
				
				byte_count += count;
				
				if (count == 0) /////////////////////////////////////////////
				{
					is_end = 1;
					close(fdin);
				}
			}



			for (i = 0; i < n * 4; i+= 4)
			{

				if (FD_ISSET(pipe_arr[i+2], &fd_read))
				{
					int k = 0;

					if ((k = read(pipe_arr[i+2], buf[i/4 + 1].data + buf[i/4 + 1].end, buf[i/4 + 1].capacity - buf[i/4 + 1].end)) == 0)
					{
						printf("Child is dead\n");
						exit(EXIT_FAILURE);
					}

					buf[i/4 + 1].end += k;
				}


				if (FD_ISSET(pipe_arr[i+1], &fd_write))
				{
					int countcpy = buf[i/4].end - buf[i/4].start;

					countcpy = write(pipe_arr[i+1], buf[i/4].data + buf[i/4].start, countcpy);

					buf[i/4].start += countcpy;
						
					if (buf[i/4].start == buf[i/4].end)
					{
						buf[i/4].start = 0;
						buf[i/4].end = 0;
					}
				}
			}

			if (FD_ISSET(1, &fd_write))
			{
				int count = buf[n].end - buf[n].start;

				count = write(1, buf[n].data + buf[n].start, count);
				buf[n].start += count;
				byte_count -= count;

				if (is_end && byte_count == 0)
					return 0;

				if (buf[n].start == buf[n].end)
				{
					buf[n].start = 0;
					buf[n].end = 0;
				}
			}
		}

	}


	else
	{
		int j = 0;
		for (j = 0; j < n; j++)
		{
			close(pipe_arr[j*4 + 2]);
			close(pipe_arr[j*4 + 1]);

			if (j != i)
			{
				close(pipe_arr[j*4 + 3]);
				close(pipe_arr[j*4]);
			}		
		}

		char buf[4096];

		while(1)
		{
			int count = read(pipe_arr[i*4], &buf, sizeof(buf));
			sleep(1);
			if (count == 0)
				return 0;
			write(pipe_arr[i*4 + 3], &buf, count);
		}
	}

	return 0;
}
