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

const char *bootsect = "boot.bin";
const char *setup = "setup.bin";
const char *kernel = "kernel.bin";

void patch_module(const char *module_name, size_t patch_size, 
		unsigned char patten, struct stat *p_stat) {

	unsigned char *buf = NULL;
	int nbytes = 0;
	int fd_module_name = -1;
	int filler_bytes = 0;

	if (lstat(module_name, p_stat) == -1)
		die("Cannot get %s attribute.\n", module_name);

	if (p_stat->st_size < patch_size) {
		fd_module_name = open(module_name, O_RDWR | O_APPEND);
		if (fd_module_name == -1) {
			die("Open %s failed.\n", module_name);
		}

		filler_bytes = patch_size - p_stat->st_size;
		buf = (unsigned char *)malloc(filler_bytes);
		if (buf == NULL)
			die("Allocation buff failed.\n");
		memset(buf, patten, filler_bytes);
		nbytes = write(fd_module_name, buf, filler_bytes);

		if (nbytes != filler_bytes)
			die("Failed to patch %s.\n", module_name);
		else {
			printf("%d bytes are patched into %s.\n", filler_bytes, module_name);
			fsync(fd_module_name);
			close(fd_module_name);
		}
		free(buf);
	}
	else if (p_stat->st_size == patch_size)
		printf("%s doesn't need to be patched.\n", module_name);
	else
		die("%s is out of %d.\n", module_name, patch_size);

}

void usage() {
	printf("build boot setup system [ROOTDEV].\n");
}

int main(int argc, char **argv) {
	char buf[1024];

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
	int fd = open(bootsect, O_RDONLY);

	if (fd == -1) {
		die("Cannot open %s for read.\n", bootsect);
	}
	if (fstat(fd, p_stat) == -1) {
		die("Cannot get %s attribute.\n", bootsect);
	}
	if (p_stat->st_size != 0x200) {
		die("Invalid %s length.\n", bootsect);	
	}

	ssize_t n_read = read(fd, buf, 512);

	if (n_read != 512) {
		die("Read %s failed.\n", bootsect);
	}
	if ((buf[510] != 0x55) || (buf[511] != 0xAA)) {
		die("Invalid %s signature.\n", bootsect);
	}
	int n_write = write(STDOUT_FILENO, buf, 512);
	if (n_write != 512) {
		die("Write %s to stdout failed.\n", bootsect);
	}
	close(fd);

	// Check setup.bin
	fd = open(setup, O_RDWR);	

	if (fd == -1) {
		die("Cannot open %s for patch.\n", setup);
	}
	if (fstat(fd, p_stat) == -1) {
		die("Cannot get %s attribute.\n", setup);
	}
	if (p_stat->st_size > SETUP_SIZE) {
		die("Illegal %s size.\n");
	}
	for ( ; n_read = read(fd, buf, 1024); ) {
		if (n_read < 0) {
			die("Read %s failed.\n", setup);
		}
		if (n_read != write(STDOUT_FILENO, buf, n_read)) {
			die("Write %s to stdout failed.\n");
		}
	}
	// Patch setup.bin
	int i = p_stat->st_size;
	int patch_bytes = 0;
	memset(buf, 0, sizeof(buf));
	while (i < SETUP_SIZE) {
		patch_bytes = SETUP_SIZE - i;
		if (patch_bytes > sizeof(buf)) {
			patch_bytes = sizeof(buf);
		}
		write(STDOUT_FILENO, buf, patch_bytes);
		i += patch_bytes;
	}
	else {
		printf("The %s doesn't need to be patched.\n", setup);
	}
	free(p_stat);

	return 0;
}
