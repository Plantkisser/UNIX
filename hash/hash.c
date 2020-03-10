#include "hash.h"


#define TEST 19
#define MAX_DELETED 4

enum State
{
	S_FULL,
	S_DELETED,
	S_EMPTY
};


enum Functions
{
	FC_INSERT,
	FC_COMMON
};




struct data
{
	char* key;
	Value_t val;
	int state;
	int was_checked;
	int counter;
};


struct hashtable
{
	Data* arr;
	int size;
	unsigned int capacity;
};


int firstHash(unsigned char* key)
{
	return key[0] % 100;
}

int secondHash(char* key)
{
	return 1;
}



static int find_and_ret(Hashtable* ht, char* user_key, int mode)
{
	int fh = firstHash(user_key);

	int k = 0;
	int sh = secondHash(user_key);

	for(k = 0; k < ht->capacity - 1; k++)
	{
		int i = (fh + k * sh) % ht->capacity;

		

		if (mode == FC_INSERT)
		{
			if (ht->arr[i].state != S_FULL)
			{
				errno = E_SUCCESS;
				return i;
			}
			if (ht->arr[i].state == S_FULL && strcmp(ht->arr[i].key, user_key) == 0)
			{
				errno = E_SAME;
				return i;
			}
			ht->arr[i].was_checked++;
			continue;
		}





		if (ht->arr[i].state == S_EMPTY)
		{
			//printf("***%d****\n", i);
			errno = E_NOTFOUND;
			return i;
		}


		if (ht->arr[i].state == S_FULL && strcmp(ht->arr[i].key, user_key) == 0)
		{
			errno = E_SUCCESS;
			return i;
		}
	}

	errno = E_NOTFOUND;

	return firstHash(user_key);
}


static int rollback(Hashtable* ht, int goal, int num, int step, int val)
{
	ht->arr[num].was_checked -= val;

	while(num != goal)
	{
		num = (num - step + ht->capacity) % ht->capacity;

		ht->arr[num].was_checked -= val;

		
		if (ht->arr[num].was_checked == 0 && ht->arr[num].state == S_DELETED)
			ht->arr[num].state = S_EMPTY;

		//printf("%d %d\n", num, goal);
	}

	return OK;
}












void* my_malloc(int obj_size, int count)
{
	#ifdef TEST
	static int k = 0;
	static int n = 7;
	k = (k % n) + 1;
	//printf("%d\n", k);
	if (k == n)
	{
		k = 0;
		n++;
		if (n > 10)
			n = 7;
		errno = E_ALLOC;
		return NULL;
	}
	
	#endif



	return malloc(obj_size * count);
}


Hashtable* createHashTable(int len)
{
	if (len < 100)
		len = 100;

	Hashtable* ht = NULL;
	if ((ht = (Hashtable*) my_malloc(sizeof(Hashtable), 1)) == NULL)
	{
		return NULL;
	}
	//printf("*\n");
	ht->capacity = len;

	if ((ht->arr = (Data*) my_malloc(sizeof(Data), ht->capacity)) == NULL)
	{
		free(ht);
		return NULL;
	}

	



	int i = 0;
	for (i = 0; i < ht->capacity; ++i)
	{
		ht->arr[i].was_checked = 0;
		ht->arr[i].state = S_EMPTY;
		ht->arr[i].counter = 0;
	}
	ht->size = 0;
	return ht;
}


int deleteHashTable(Hashtable* ht)
{
	if (!ht || !ht->arr) 
	{
		errno = E_NULL;
		return ERROR;
	}

	int i = 0;
	for (i = 0; i < ht->capacity; ++i)
		if (ht->arr[i].state == S_FULL &&  ht->arr[i].key)
			free(ht->arr[i].key);


	free(ht->arr);
	free(ht);
	return OK;
}


int insert(Hashtable* ht, char* key, Value_t val)
{
	if (!ht || !ht->arr || !key)
	{
		errno = E_NULL;
		return ERROR;
	}


	int i = find_and_ret(ht, key, FC_INSERT);

	if (errno == E_NOTFOUND)
	{
		int fh = firstHash(key);
		int sh = secondHash(key);



		//printf("rollback %d\n", (i - sh + ht->capacity) % ht->capacity);
		rollback(ht, fh, (i - sh + ht->capacity) % ht->capacity, sh, 1);
		return ERROR;
	}

	if (errno == E_SAME)
	{
		int fh = firstHash(key);
		int sh = secondHash(key);

		if (i != fh) rollback(ht, fh, (i - sh + ht->capacity) % ht->capacity, sh, 1);
		return ERROR;	
	}





	ht->arr[i].key = my_malloc(sizeof(int), strlen(key) + 1);
	if (ht->arr[i].key == NULL)
	{
		return ERROR;
	} 
	


	ht->arr[i].state = S_FULL;
	
	strcpy(ht->arr[i].key, key); 
	ht->arr[i].val = val;

	ht->size++;

	ht->arr[i].counter = 1;







	#ifdef TEST
	printf("insert: %d\n", i);
	#endif
	return OK;
}



int find(Hashtable* ht, char* user_key, Value_t* obj)
{
	if (!ht || !ht->arr || !user_key)
	{
		errno = E_NULL;
		return ERROR;
	}

	int i = find_and_ret(ht, user_key, FC_COMMON);
	
	if (errno == E_NOTFOUND)
	{
		#ifdef TEST
		printf("FIND %d\n", i);
		#endif
		return FALSE;
	}


	if (obj)
		*obj = ht->arr[i].val;

	return TRUE;

}


int deleteObj(Hashtable* ht, char* user_key)
{
	if (!ht || !ht->arr || !user_key)
	{
		errno = E_NULL;
		return ERROR;
	}

	int i = find_and_ret(ht, user_key, FC_COMMON);
	


	if (errno == E_NOTFOUND)
	{
		return FALSE;
	}
	
	
	ht->arr[i].state = S_DELETED;
	ht->size--;
	free(ht->arr[i].key);


	int fh = firstHash(user_key);
	int sh = secondHash(user_key);

	if (i != fh)
		rollback(ht, fh, (i - sh + ht->capacity) % ht->capacity, sh, ht->arr[i].counter);

	ht->arr[i].counter = 0;




	if (ht->arr[i].was_checked == 0)
	{
		ht->arr[i].state = S_EMPTY;
	}


	return TRUE;	
}



int cleanHashTable(Hashtable* ht)
{
	if (!ht)
	{
		errno = E_NULL;
		return ERROR;
	}


	int i = 0;

	for (i = 0; i < ht->capacity; ++i)
	{
		if (ht->arr[i].state == S_FULL)
			free(ht->arr[i].key);
		ht->arr[i].state = S_EMPTY;
		ht->arr[i].counter = 0;
		ht->arr[i].was_checked = 0;
	}

	ht->size = 0;
	return OK;
}



int foreach(Hashtable* ht, int (*cb)(char*, Value_t*, void*), void* smth)
{
	if (!ht || !cb)
	{
		errno = E_NULL;
		return ERROR;
	}


	int i = 0;
	for (i = 0; i < ht->capacity; ++i)
	{

		if (ht->arr[i].state != S_FULL)
			continue;
		if (cb(ht->arr[i].key, &(ht->arr[i].val), smth) != OK)
			return ERROR;
	}
	return OK;
}

