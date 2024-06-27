/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <android-base/logging.h>
#include <errno.h>
#include <sys/time.h>
#include "mutex-inl.h"
#include <pthread.h>

volatile int addr = 1;
volatile int addr2 = 2;
volatile int32_t cur_sequence = 1;

__attribute__((noinline))
void* myFunction(void* arg) {
   LOG(INFO) << "futex wait in ";
  if (futex(&addr, FUTEX_WAIT_PRIVATE, cur_sequence, nullptr, nullptr, 0) != 0) {
    // Futex failed, check it is an expected error.
    // EAGAIN == EWOULDBLK, so we let the caller try again.
    // EINTR implies a signal was sent to this thread.
    if ((errno != EINTR) && (errno != EAGAIN)) {
       sigset_t pendmask;
       if (sigpending64(&pendmask) >= 0)  {
	 const unsigned long* set = reinterpret_cast<const unsigned long*>(&pendmask);
	 LOG(ERROR) << "pending sig=" << set[0] << " " << set[1];
       } else {
	 LOG(ERROR) << "pending err";
       }
      PLOG(FATAL) << "futex wait failed:errno=" << errno;
    }
  } else {
    LOG(INFO) << "futex wait ok ";
  }
  return nullptr;
}

int main(int argc, char** argv) {
    pthread_t t1;
    pthread_t t2;
    int param = 42;
 
    if(pthread_create(&t1, NULL, myFunction, &param) != 0) {
        LOG(ERROR) <<"Failed to create thread 1";
        return 1;
    }
 
    if(pthread_create(&t2, NULL, myFunction, &param) != 0) {
        LOG(ERROR) << "Failed to create thread 2";
        return 1;
    }
    
    sleep(2);

    //asm volatile(".word 0x002a0000 \n\t"); //break 0x0 and break 0x4 can cause futex_wait return errno=512
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

    LOG(INFO) << "Main thread exiting";
    return 0;
}
