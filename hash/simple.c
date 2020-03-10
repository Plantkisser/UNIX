#include <stdio.h>



int main()
{
	int i = 4;

	while (i * i <= 1300)
	{
		printf("%d\n", i*i);
		i++;
	}




	printf("KUB:\n");

	i = 3;
	while (i * i * i <= 1300)
	{
		printf("%d\n", i * i * i);
		i++;
	}
}