#ifdef _WIN32
#include <windows.h>
#include <stdint.h>
#include <stdio.h>

typedef HANDLE pthread_t;
static inline int pthread_detach(pthread_t t) { (void)t; return 0; }
static inline pthread_t pthread_self(void) { return GetCurrentThread(); }
static inline void pthread_exit(void *ret) { ExitThread((DWORD)(intptr_t)ret); }
#else
#include <pthread.h>
#include <stdio.h>
#endif

#define STACK_SIZE 1024 * 1024

void* thread(void *arg) {
    pthread_detach(pthread_self());
    pthread_exit(arg);
}

