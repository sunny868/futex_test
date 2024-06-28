#ifndef FUTEX_INL_H_
#define FUTEX_INL_H_
#include <unistd.h>
#include <sys/syscall.h>
#ifndef SYS_futex
#define SYS_futex __NR_futex
#endif


static inline int futex(volatile int *uaddr, int op, int val, const struct timespec *timeout,
                        volatile int *uaddr2, int val3) {
  return syscall(SYS_futex, uaddr, op, val, timeout, uaddr2, val3);
}

#endif  // FUTEX_INL_H_
