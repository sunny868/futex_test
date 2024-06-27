# LA编译方式

## 1. 源码下载到aosp/external/下
 
## 2. 编译
 
  在aosp目录下执行
```sh
  $ make futex_test
```
## 3. 将编译后的futex_test二进制文件拷贝到qemu的/data目录

```sh
 $ adb push out/target/product/generic_loongarch64/system/bin/futex_test /data. 
```

## 4. qemu内启动程序

```sh
console:/ # setprop log.tag.futex_test V
console:/ # cd /data
console:/data # gdbserver64 --no-startup-with-shell 127.0.0.1:5039 ./futex_test
```

## 5. gdb client连接并运行

```sh
(gdb) target extended-remote :5039
Remote debugging using :5039
No executable file now.
warning: Could not load vsyscall page because no executable was specified
0x00007fffd9d873e0 in ?? ()
(gdb) c
Continuing.
[New Thread 1574.1685]
[New Thread 1574.1686]

Thread 1 "futex_test" received signal SIGINT, Interrupt.
0x00007fffd3ee1828 in ?? ()
(gdb) c
Continuing.

Thread 3 "futex_test" received signal SIGABRT, Aborted.
[Switching to Thread 1574.1686]
0x00007fffd3e83200 in ?? ()
(gdb) 
```
在Loongarch架构下这里报错SIGABRT,是因为代码中走到了`PLOG(FATAL)`位置。对比log信息：

```sh
06-27 10:38:09.836  1574  1685 E futex_test: pending sig=0 0
06-27 10:38:09.838  1574  1685 F futex_test: futex wait failed:errno=512: Unknown error 512
06-27 10:38:09.839  1574  1686 E futex_test: pending sig=0 0
06-27 10:38:09.840  1574  1686 F futex_test: futex wait failed:errno=512: Unknown error 512
```
# X86编译/运行

同上述步骤。
X86上不会报错SIGABRT
