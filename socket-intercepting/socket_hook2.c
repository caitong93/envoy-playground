// Copied from https://fishi.devtail.io/weblog/2015/01/25/intercepting-hooking-function-calls-shared-c-libraries/

/*
* A simple socket hook which calls the original socket library.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
    
int (*o_socket)(int,int,int);

static __thread int initialized;
    
int socket(int domain, int type, int protocol)
{
    // find the next occurrence of the socket() function
    o_socket = dlsym(RTLD_NEXT, "socket");
    
    if(o_socket == NULL)
    {
        printf("Could not find next socket() function occurrence");
        return -1;
    }

    if (!initialized) {
        pid_t tid = syscall(__NR_gettid);
        initialized = !initialized;

        printf("[%d]socket() call intercepted\n", tid);
        printf("[%d]sleep in first socket() call\n", tid);
        sleep(5);
        printf("[%d]sleep end\n", tid);
    }
    
    // return the result of the call to the original C socket() function
    return o_socket(domain,type,protocol);
}