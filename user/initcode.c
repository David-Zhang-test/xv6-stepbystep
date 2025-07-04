#include "user.h"

char *argv1[] = {"execchild1", 0};
char *argv2[] = {"execchild2", 0};

int main()
{
	int pid, to_be_killed;

	// child 1: 100% CPU
	// test for: fork, time slice, kill
	pid = fork();
	if (pid < 0)
	{
		printf("[pid: %d] 1st fork() failed", getpid());
		return 0;
	}
	if (pid == 0)
	{
		exec("execchild1", argv1);
		exit(0);
	}
	to_be_killed = pid;

	// child 2: dynamic memory allocation
	// test for: fork, getpid, uptime, write, sbrk, exit
	pid = fork();
	if (pid < 0)
	{
		printf("[pid: %d] 2nd fork() failed", getpid());
		return 0;
	}
	if (pid == 0)
	{
		exec("execchild2", argv2);
		exit(0);
	}

	// child 3: sleep test
	// test for: fork, getpid, uptime, write, sleep, exit, kill
	pid = fork();
	if (pid < 0)
	{
		printf("[pid: %d] 2nd fork() failed", getpid());
		return 0;
	}
	if (pid == 0)
	{
		sleep(10);
		printf("[pid: %d] sleep test - 1s. uptime: %d\n", getpid(), uptime());
		sleep(20);
		printf("[pid: %d] sleep test - 2s. uptime: %d\n", getpid(), uptime());
		sleep(30);
		printf("[pid: %d] sleep test - 3s. uptime: %d\n", getpid(), uptime());
		kill(to_be_killed);
		exit(0);
	}
	// init process: wait
	// test for: wait, getpid, uptime, write
	while (1)
	{
		int pid, xstate;
		pid = wait(&xstate);
		if (pid < 0)
			break;
		printf("[pid: %d] wait - pid: %d - exit code: %d\n", getpid(), pid, xstate);
	}
	printf("[pid: %d] uptime: %d, init process has no child now, over!\n", getpid(), uptime());
	while (1)
		;
	exit(0);
}
