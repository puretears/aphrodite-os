#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>

#define SETUP_SIZE 2048
#define KERNEL_SIZE 192 * 1024

void die(char *err_info, ...) {
	va_list ap;
	va_start(ap, err_info);
	vprintf(err_info, ap);
	va_end(ap);
	printf("%s", err_info);
	exit(-1);
}

void usage() {
	printf("build boot setup system [ROOTDEV].\n");
}

int main(int argc, char **argv) {
	unsigned char buf[1024];

	if ((argc != 4) && (argc != 5)) {
		usage();
		exit(-1);
	}
	if (argc == 5) {
	
	}
	else {
		
	}

	// Check boot.bin
	struct stat *p_stat = (struct stat *)malloc(sizeof(struct stat));
	int fd = open(argv[1], O_RDONLY);

	if (fd == -1) {
		die("Cannot open %s for read.\n", argv[1]);
	}
	if (fstat(fd, p_stat) == -1) {
		die("Cannot get %s attribute.\n", argv[1]);
	}
	if (p_stat->st_size != 0x200) {
		die("Invalid %s length.\n", argv[1]);	
	}

	ssize_t n_read = read(fd, buf, 512);

	if (n_read != 512) {
		die("Read %s failed.\n", argv[1]);
	}
	if ((buf[510] != 0x55) || (buf[511] != 0xAA)) {
		die("Invalid %s signature %01X%01X.\n", argv[1], buf[1], buf[0]);
	}
	int n_write = write(STDOUT_FILENO, buf, 512);
	if (n_write != 512) {
		die("Write %s to stdout failed.\n", argv[1]);
	}
	close(fd);

	// Deal with setup.bin
	fd = open(argv[2], O_RDWR);	

	if (fd == -1) {
		die("Cannot open %s for patch.\n", argv[2]);
	}
	if (fstat(fd, p_stat) == -1) {
		die("Cannot get %s attribute.\n", argv[2]);
	}
	if (p_stat->st_size > SETUP_SIZE) {
		die("Illegal %s size.\n", argv[2]);
	}
	for ( ; (n_read = read(fd, buf, 1024)); ) {
		if (n_read < 0) {
			die("Read %s failed.\n", argv[2]);
		}
		if (n_read != write(STDOUT_FILENO, buf, n_read)) {
			die("Write %s to stdout failed.\n");
		}
	}
	// Patch setup.bin
	int i = p_stat->st_size;
	int patch_bytes = 0;
	int total_patch = 0;

	memset(buf, 0, sizeof(buf));
	while (i < SETUP_SIZE) {
		patch_bytes = SETUP_SIZE - i;
		total_patch += patch_bytes;
		if (patch_bytes > sizeof(buf)) {
			patch_bytes = sizeof(buf);
		}
		write(STDOUT_FILENO, buf, patch_bytes);
		i += patch_bytes;
	}
	
	sprintf(buf, "%d bytes are patched into %s.\n", total_patch, argv[1]);
	perror(buf);
	close(fd);

	// Deal with system module
	fd = open(argv[3], O_RDONLY);

	if (fd == -1) {
		die("Cannot open %s for read.\n", argv[3]);
	}
	if (fstat(fd, p_stat) == -1) {
		die("Cannot get %s attribute.\n", argv[3]);
	}
	if (p_stat->st_size > KERNEL_SIZE) {
		die("Illegal %s size.\n", argv[3]);
	}
	for ( ; (n_read = read(fd, buf, 1024)); ) {
		if (n_read < 0) {
			die("Read %s failed.\n", argv[3]);
		}
		if (n_read != write(STDOUT_FILENO, buf, n_read)) {
			die("Write %s to stdout failed.\n", argv[3]);
		}

	}
	close(fd);
	free(p_stat);

	return 0;
}
