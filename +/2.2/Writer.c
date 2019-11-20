#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
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
	if(argc != 2)
	{
		printf("ERROR ARG\n");
		return 0;
	}


	int key = ftok("/home/kihu/Desktop/lun/+/2.2/l", 4);


	int sem_set = semget(key, 4, IPC_CREAT | IPC_EXCL | 0666); // W R SYNC PAIR
	if (sem_set == -1) sem_set = semget(key , 4, 0666);


	struct sembuf buf[4];

	buf[0].sem_num = SEM_WRITER;
	buf[0].sem_op = 0;
	buf[0].sem_flg = 0;

	buf[1].sem_num = SEM_WRITER;
	buf[1].sem_op = 1;
	buf[1].sem_flg = SEM_UNDO;

	buf[2].sem_num = SEM_PAIR;
	buf[2].sem_op = 0;
	buf[2].sem_flg = 0;	


	semop(sem_set, buf, 3);	

	buf[0].sem_num = SEM_READER;
	buf[0].sem_op = -1;
	buf[0].sem_flg = 0;

	buf[1].sem_num = SEM_READER;
	buf[1].sem_op = 1;
	buf[1].sem_flg = 0;

	buf[2].sem_num = SEM_PAIR;
	buf[2].sem_op = 1;
	buf[2].sem_flg = SEM_UNDO;

	semop(sem_set, buf, 3);	



	int shm_id = shmget(key, 100, 0666);



	char* shm_ptr = (char*) shmat(shm_id, NULL, 0);
	
	if(*shm_ptr == (char) -1)
		return 0;
	


	memset(shm_ptr, 0, 100);


	int fdin = open(argv[1], O_RDONLY);

	int first = 0;



	/////////////////////////////////////////////////////////////////////////////
	

	buf[0].sem_num = SEM_SYNC;
	buf[0].sem_op = 1;
	buf[0].sem_flg = SEM_UNDO;

	buf[1].sem_num = SEM_SYNC;
	buf[1].sem_op = -1;
	buf[1].sem_flg = 0;

	semop(sem_set, buf, 2);	


	//////////////////////////////////////////////////////////////////////////////
	

	do 
	{
		buf[2].sem_num = SEM_SYNC;
		buf[2].sem_op = 0;
		buf[2].sem_flg = 0;

		buf[0].sem_num = SEM_READER;
		buf[0].sem_op = -1;
		buf[0].sem_flg = IPC_NOWAIT;

		buf[1].sem_num = SEM_READER;
		buf[1].sem_op = 1;
		buf[1].sem_flg = IPC_NOWAIT;


		/*printf("SLEEP\n");
		sleep(5);*/
		int ress = semop(sem_set, buf, 3);





		if (ress == -1)
		{
			break;
		}
	
		
		if(shm_ptr[0] != 0)
		{
			printf("ERROR* %d\n", shm_ptr[0]);
			break;
		}


		int res = read(fdin, shm_ptr + 1, 99);
		
		if (res == -1)
		{
			printf("ERROR\n");
			break;
		}

		buf[2].sem_num = SEM_SYNC;
		buf[2].sem_op = 1;
		buf[2].sem_flg = 0;

		buf[0].sem_num = SEM_READER;
		buf[0].sem_op = -1;
		buf[0].sem_flg = IPC_NOWAIT;

		buf[1].sem_num = SEM_READER;
		buf[1].sem_op = 1;
		buf[1].sem_flg = IPC_NOWAIT;

		shm_ptr[0] = res + 1;
		
		ress = semop(sem_set, buf, 3);


		
		if (ress == -1)
			break;
		

		if (res < 99) // finish of writing
			break;


	} while (1);

	shmdt(shm_ptr);


	buf[0].sem_num = SEM_WRITER;
	buf[0].sem_op = -1;
	buf[0].sem_flg = SEM_UNDO;

	buf[1].sem_num = SEM_PAIR;
	buf[1].sem_op = -1;
	buf[1].sem_flg = SEM_UNDO;

	semop(sem_set, buf, 2);

	return 0;
}
