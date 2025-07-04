#include "user.h"

int main()
{
	write(3, 0, 0); // 3 is testcode for block r/w, see sysfile.c sys_write()
	while (1)
		;
	exit(0);
}
