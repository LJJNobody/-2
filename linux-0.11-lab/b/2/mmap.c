/*
 * 测试成功时输出：
 * "  Hello, mmap success."
 * 测试失败时输出：
 * "mmap error."
 */
#define __LIBRARY__
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "new.h"

_syscall2(int,munmap,void *,addr,size_t,length)

void *mmap(void *addr, size_t length, ...)
{
	register int res;
	va_list arg;

	va_start(arg,length);
	__asm__("int $0x80"
		:"=a" (res)
		:"0" (__NR_open),"b" (addr),"c" (length),
		"d" (va_arg(arg,int)));
	if (res>=0)
		return (void *)res;
	errno = -res;
	return (void *)-1;
}

static struct stat kst;
void test_mmap(void){
    char *array;
    const char *str = "  Hello, mmap success.";
    int fd;

    fd = open("test_mmap.txt", O_RDWR | O_CREAT, S_IRUSR|S_IWUSR);
    write(fd, str, strlen(str));
    fstat(fd, &kst);
    printf("file len: %d\n", (int)kst.st_size);
    array = mmap(NULL, kst.st_size, PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    printf("mmap addr: %x\n", (unsigned int)array);

    if (array == MAP_FAILED) {
	    printf("mmap error.\n");
    }else{
	    printf("mmap content: %s\n", array);
    	munmap(array, kst.st_size);
    }

    close(fd);
}

int main(void){
    test_mmap();
    return 0;
}
