#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>



enum
{
	SEM_WRITER,
	SEM_READER,
	SEM_SYNC,
	SEM_PAIR
};


int main(int argc, char* argv[])
{
		

	//sleep(2);
	int key = ftok("/home/kihu/Desktop/lun/+/2.2/l", 4);

	int shm_id = shmget(key, 100, IPC_CREAT | IPC_EXCL | 0666);
	if(shm_id == -1)
		shm_id = shmget(key, 100, 0666);



	int sem_set = semget(key, 4, IPC_CREAT | IPC_EXCL | 0666); // W R SYNC PAIR
	if (sem_set == -1) sem_set = semget(key, 4, 0666);

	struct sembuf buf[4];

	buf[0].sem_num = SEM_READER;
	buf[0].sem_op = 0;
	buf[0].sem_flg = 0;

	buf[1].sem_num = SEM_READER;
	buf[1].sem_op = 1;
	buf[1].sem_flg = SEM_UNDO;

	buf[2].sem_num = SEM_PAIR;
	buf[2].sem_op = 0;
	buf[2].sem_flg = 0;


	semop(sem_set, buf, 3);	

	buf[0].sem_num = SEM_WRITER;
	buf[0].sem_op = -1;
	buf[0].sem_flg = 0;

	buf[1].sem_num = SEM_WRITER;
	buf[1].sem_op = 1;
	buf[1].sem_flg = 0;

	buf[2].sem_num = SEM_PAIR;
	buf[2].sem_op = 1;
	buf[2].sem_flg = SEM_UNDO;

	semop(sem_set, buf, 3);	



	char* shm_ptr = (char*) shmat(shm_id, NULL, 0);
	if(*shm_ptr == (char) -1)
		return 0;

	int res = 0;
	

	do
	{
		buf[0].sem_num = SEM_WRITER;
		buf[0].sem_op = -1;
		buf[0].sem_flg = IPC_NOWAIT;

		buf[1].sem_num = SEM_WRITER;
		buf[1].sem_op = 1;
		buf[1].sem_flg = IPC_NOWAIT;

		buf[2].sem_num = SEM_SYNC;
		buf[2].sem_op = -1;
		buf[2].sem_flg = 0;

		buf[3].sem_num = SEM_SYNC;
		buf[3].sem_op = 1;
		buf[3].sem_flg = SEM_UNDO;

		if (semop(sem_set, buf, 4) == -1)
			break;


		if (shm_ptr[0] != 0) 
			write(1, shm_ptr + 1, (int) shm_ptr[0] - 1);
		
		if (shm_ptr[0] < 100 && shm_ptr[0] > 0)
		{

			memset(shm_ptr, 0, 100);

			buf[0].sem_num = SEM_SYNC;
			buf[0].sem_op = -1;
			buf[0].sem_flg = 0;
			semop(sem_set, buf, 1);
			break;
		}
		
		memset(shm_ptr, 0, 100);

		buf[0].sem_num = SEM_WRITER;
		buf[0].sem_op = -1;
		buf[0].sem_flg = IPC_NOWAIT;

		buf[1].sem_num = SEM_WRITER;
		buf[1].sem_op = 1;
		buf[1].sem_flg = IPC_NOWAIT;

		buf[2].sem_num = SEM_SYNC;
		buf[2].sem_op = -1;
		buf[2].sem_flg = SEM_UNDO;

		/*printf("SLEEP\n");
		sleep(3);*/
		res = semop(sem_set, buf, 3);
		/*printf("SLEEP\n");
		sleep(3);*/
		if(res == -1)
			break;
		
	} while(1);
	shmdt(shm_ptr);

	buf[0].sem_num = SEM_READER;
	buf[0].sem_op = -1;
	buf[0].sem_flg = SEM_UNDO;

	buf[1].sem_num = SEM_PAIR;
	buf[1].sem_op = -1;
	buf[1].sem_flg = SEM_UNDO;

	semop(sem_set, buf, 2);	
	
	return 0;
}