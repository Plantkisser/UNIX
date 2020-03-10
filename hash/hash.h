#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>



#define TRUE 1
#define FALSE 0
#define ERROR -1
#define OK 0


typedef int Value_t;
typedef struct hashtable Hashtable;
typedef struct data Data;


enum Errors
{
	E_NOTFOUND,
	E_SAME,
	E_ALLOC,
	E_NULL,
	E_FULL,
	E_SUCCESS
};


struct hashtable;
struct Data;

Hashtable* createHashTable(int len);
int deleteHashTable(Hashtable* ht);
int insert(Hashtable* ht, char* key, int val);
int find(Hashtable* ht, char* user_key, Value_t* obj);
int deleteObj(Hashtable* ht, char* user_key);
int cleanHashTable(Hashtable* ht);
//Hashtable* increaseHashTable(Hashtable* ht);

int foreach(Hashtable* ht, int (*cb)(char*, Value_t*, void*), void* smth);




