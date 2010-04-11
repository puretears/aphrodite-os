#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>

#define SETUP_SIZE 2048
#define KERNEL_SIZE 192 * 1024

#pragma warning(disable:)

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

int main() {
	char *buf;

	struct stat *p_stat = (struct stat *)malloc(sizeof(struct stat));
	if (lstat(bootsect, p_stat) == -1) {
		die("Cannot get bootsect attribute.");
	}

	if (p_stat->st_size != 0x200) {
		die("Invalid bootsect length.");	
	}
	patch_module(setup, SETUP_SIZE, 0, p_stat);
	patch_module(kernel, KERNEL_SIZE, 'A', p_stat);
	free(p_stat);

}
