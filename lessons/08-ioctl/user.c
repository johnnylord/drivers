#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "message.h"


#define DEVICE "/dev/ioctl"


int main(int argc, char *argv[])
{
	int fd;
	long ret;
	struct message msg = { 0 };

	fd = open(DEVICE, O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(1);
	}

	msg.order = 10;

	ret = ioctl(fd, MESSAGE_POS, &msg);
	if (ret < 0) {
		perror("ioctl");
		exit(1);
	}

	printf("Order %u equals to %lu\n", msg.order, msg.value);

	return 0;
}
