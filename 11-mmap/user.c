#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define DEVICE "/dev/kmem"
#define RESERVED_SIZE 4096
#define BUF_SIZE 4096

int main(int argc, char *argv[])
{
	int fd;
	char *pmap;
	char buffer[BUF_SIZE];

	// Get the device file descriptor
	fd = open(DEVICE, O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(1);
	}
	printf("Open device file %s\n", DEVICE);

	// Perform memory mapping on the device
	pmap = mmap(0, RESERVED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (pmap == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(1);
	}
	printf("Memory mapping succeed\n");

	// Read mmaped memory (from kernel space) directly
	strncpy(buffer, pmap, BUF_SIZE);
	printf("Original content: %s\n", buffer);

	/* Convert charactor from lower to upper */
	for (int i=0; i<BUF_SIZE; i++) {
		if (buffer[i] >= 97 && buffer[i] <= 122) {
			buffer[i] -= 32;
		} else {
			buffer[i] += 32;
		}
	}

	// Wrtie mmaped memory (from kernel space) directly
	strncpy(pmap, buffer, BUF_SIZE);

	// Read mmaped memory (from kernel space) directly
	strncpy(buffer, pmap, BUF_SIZE);
	printf("Modified buffer: %s\n", buffer);

	close(fd);
	munmap(pmap, RESERVED_SIZE);

	return 0;
}
