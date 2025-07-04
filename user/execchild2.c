#include "user.h"

int main()
{
	int *p[10];
	for (int i = 0; i < 10; i++)
		p[i] = malloc((i + 1) * 1000);
	// --------
	free(p[0]);
	free(p[2]);
	free(p[4]);
	free(p[6]);
	free(p[8]);
	// --------
	free(p[9]);
	free(p[7]);
	free(p[5]);
	free(p[3]);
	free(p[1]);
	printf("[pid: %d] sbrk test over! uptime: %d\n", getpid(), uptime());
	exit(0);
}
