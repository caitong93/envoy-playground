// Copied from https://fishi.devtail.io/weblog/2015/01/25/intercepting-hooking-function-calls-shared-c-libraries/

/*
* A simple socket hook which calls the original socket library.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/socket.h>
#include <dlfcn.h>
    
int (*o_socket)(int,int,int);

int socket(int domain, int type, int protocol)
{
    // find the next occurrence of the socket() function
    o_socket = dlsym(RTLD_NEXT, "socket");
    
    if(o_socket == NULL)
    {
        printf("Could not find next socket() function occurrence");
        return -1;
    }
    
    printf("socket() call intercepted\n");

    // return the result of the call to the original C socket() function
    return o_socket(domain,type,protocol);
}