#include "hash.h"


int sum(char* key, Value_t* val, void* counter)
{
	if (!key || !counter || !val)
	{
		errno = E_NULL;
		return ERROR;
	}
	int* c = (int*) counter;
	(*c)++;

	return OK;
}



int main()
{
	Hashtable* ptr = NULL;

	char name[20];
	strcpy(name, "aello");
	int val = 1098;

	ptr = createHashTable(0);


	if (insert(ptr, name, val) == ERROR && errno == E_ALLOC);
		//insert(ptr, name, val);

	insert(ptr, NULL, 0);
	if (errno == E_NULL)
	{
		errno = E_SUCCESS;
		printf("1) insert E_NULL: checked\n");
	}

	deleteHashTable(NULL);
	if (errno == E_NULL)
	{
		errno = E_SUCCESS;
		printf("2) deleteHashTable E_NULL: checked\n");
	}

	deleteObj(NULL, NULL);
	if (errno == E_NULL)
	{
		errno = E_SUCCESS;
		printf("3) deleteObj E_NULL: checked\n");
	}




	find(NULL, "HELLO", NULL);
	if (errno == E_NULL)
	{	
		errno = E_SUCCESS;
		printf("4)find E_NULL: checked\n");
	}


	cleanHashTable(NULL);
	if (errno == E_NULL)
	{
		errno = E_SUCCESS;
		printf("5) cleanHashTable E_NULL: checked\n");
	}

	foreach(ptr, sum, NULL);
	if (errno == E_NULL)
	{
		errno = E_SUCCESS;
		printf("6)foreach first E_NULL: checked\n");
	}

	foreach(NULL, NULL, NULL);
	if (errno == E_NULL)
	{
		errno = E_SUCCESS;
		printf("7) foreach second E_NULL: checked\n");
	}

	sum(NULL, NULL, NULL);
	if (errno == E_NULL)
	{
		errno = E_SUCCESS;
		printf("8) sum E_NULL: checked\n");
	}



	char new[257][20];
	int arrval[257];

	int i = 0;

	for (i = 30; i < 257; ++i)
	{
		strcpy(new[i], "hello");
		new[i][5] = '\0';
		new[i][0] = i % 256;
		arrval[i] = i;

		if (insert(ptr, new[i], arrval[i]) == ERROR && errno == E_ALLOC)
			insert(ptr, new[i], arrval[i]);
	}





	if (errno == E_NOTFOUND) printf("9) insert E_FULL: checked\n");




	/////////////////////////



	find(ptr, "hello", &val);
	if (val == (int)'h') 
		printf("10) find work: OK\n");

	if (find(ptr, new[32], NULL) == TRUE && find(ptr, new[42], NULL) == TRUE) //Проверка работоспособоности find deleteObj
		printf("11) found element: OK\n");
	deleteObj(ptr, new[32]);
	deleteObj(ptr, new[32]);

	deleteObj(ptr, new[42]);

	deleteObj(ptr, new[32]);

	if (find(ptr, new[32], NULL) == FALSE)
		printf("12) Element wasn't found: OK\n"); 
	cleanHashTable(ptr);
	if (find(ptr, new[42], NULL) == FALSE)
		printf("13) Element wasn't found: OK\n");




	new[32][0] = 'a';
	new[32][2] = 'j';
	if (insert(ptr, new[32], arrval[32]) == ERROR);
		//insert(ptr, new[32], arrval[32]);
	new[32][2] = 'y';
	if (insert(ptr, new[32], arrval[32]) == ERROR)
		insert(ptr, new[32], arrval[32]);



	new[32][2] = 'j';
	deleteObj(ptr, new[32]);

	new[32][2] = 'y';
	deleteObj(ptr, new[32]);

	new[32][2] = 'l';
	find(ptr, new[32], NULL);

	/////////////////////////

	int counter = 0;

	if (insert(ptr, new[32], arrval[32]) == ERROR);
		//insert(ptr, new[32], arrval[32]);

	foreach(ptr, sum, (void*) &counter);


	printf("14) SUM: %d\n", counter);








	Hashtable* arr[20];
	for (i = 0; i < 20; ++i)
	{
		if ((arr[i] = createHashTable(0)) == NULL)
		{
			if (errno == E_ALLOC)
				printf("15) createHashTable E_ALLOC: checked\n");
			arr[i] = createHashTable(0);
		}
		deleteHashTable(arr[i]);
	}


	cleanHashTable(ptr);
//------------------Optimization----------------------------------------------------------------------------
	printf("\n\n\nOptimization\n");
	char* str[3];
	str[0] = "abc";
	str[1] = "abcd";
	str[2] = "abcde";

	insert(ptr, str[0], 10);
	insert(ptr, str[1], 20);
	insert(ptr, str[2], 30);

	find(ptr, "b", NULL);
	deleteObj(ptr, "b");

	deleteObj(ptr, str[0]);
	deleteObj(ptr, str[1]);
	deleteObj(ptr, str[2]);

	find(ptr, "a", NULL);



	printf("TEST\n");

	if (insert(ptr, str[1], 30) == ERROR)
		insert(ptr, str[1], 30);









	deleteHashTable(ptr);

	return 0;
}