#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#define WQ_DEV "/dev/wq"

int main(int argc, char **argv)
{
	int fd = -1;
	ssize_t retval = -1;
	size_t data = 0;

	fd = open(WQ_DEV, O_RDWR);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	retval = read(fd, (void *)&data, sizeof(size_t));
	fprintf(stderr, "retval=%u, data=0x%x\n", retval, data);

	close(fd);

	return 0;
}
