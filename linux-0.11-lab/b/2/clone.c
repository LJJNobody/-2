/*
 * 测试成功时输出：
 *   "clone success."
 * 测试失败时输出：
 *   "clone error."
 */
#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <signal.h>
#include "new.h"

int clone(int (*fn)(void *), void *child_stack, ...)
{
	register int res;
	va_list arg;

	va_start(arg,child_stack);
	__asm__("int $0x80"
		:"=a" (res)
		:"0" (__NR_clone),"b" (fn),"c" (child_stack),
		"d" (va_arg(arg,int)));
	if (res>=0)
		return res;
	errno = -res;
	return -1;
}

int g_shared = 0;
size_t stack[1024] = {0};
int child_pid;

static int child_func(void * arg){
    g_shared = 1;
    printf("  Child says hello.\n");
    exit(0);
}

void test_clone(void){
    int wstatus, ret;
    child_pid = clone(child_func, stack+1024, SIGCHLD|CLONE_VM);
    assert(child_pid != -1);

    if (child_pid == 0){
        exit(0);
    }
    printf("child pid: %d\n", child_pid);

	ret = wait(&wstatus);
	if(ret == child_pid) {
        if (g_shared == 1) {
            printf("clone success.\n");                
        }
    }
	else {
        printf("clone error.\n");    
    }
}

int main(void){
    test_clone();
    return 0;
}
