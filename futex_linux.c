#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include "futex_linux.h"
#include <pthread.h>
#include <linux/futex.h>

volatile int addr = 1;
volatile int addr2 = 2;

__attribute__((noinline))
void* myFunction(void* arg) {
   printf("futex wait in \n");
  if (futex(&addr, FUTEX_WAIT_PRIVATE, 1/*cur_sequence*/, NULL, NULL, 0) != 0) {
    if ((errno != EINTR) && (errno != EAGAIN)) {
      printf("futex wait failed:errno=%d\n" ,errno);
    }
  }
  return NULL;
}

int main(int argc, char** argv) {
    pthread_t t1;
    pthread_t t2;
    int param = 42;
 
    if(pthread_create(&t1, NULL, myFunction, &param) != 0) {
        printf("Failed to create thread 1\n");
        return 1;
    }
 
    if(pthread_create(&t2, NULL, myFunction, &param) != 0) {
        printf( "Failed to create thread 2\n");
        return 1;
    }
    sleep(2);

    kill(getpid(), SIGINT);
#if 0
    int32_t count = std::numeric_limits<int32_t>::max();
    bool done = futex(&addr,
                      FUTEX_REQUEUE_PRIVATE,
                      /* Threads to wake */ 2,
                      /* Threads to requeue*/ reinterpret_cast<const timespec*>(count),
                      &addr2,
                      0) != -1;
    if (!done && errno != EAGAIN && errno != EINTR) {
       PLOG(FATAL) << "futex requeue failed errno=" << errno;
    }
#endif

    // 等待新线程执行结束
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
 
    printf("Main thread exiting");
    return 0;
}
