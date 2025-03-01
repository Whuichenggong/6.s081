#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    char buf[5]; // 缓冲区大小为 5，包括空字符
    int fd[2];

    // 创建管道
    if (pipe(fd) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    // 创建子进程
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // 子进程：读取父进程发送的消息
        if (read(fd[0], buf, 4) < 0) {
            fprintf(2, "read failed\n");
            exit(1);
        }
        buf[4] = '\0'; // 添加空字符
        printf("%d: received %s\n", getpid(), buf);
        close(fd[0]); // 关闭读端

        // 向父进程发送消息
        if (write(fd[1], "pong", 4) < 0) {
            fprintf(2, "write failed\n");
            exit(1);
        }
        close(fd[1]); // 关闭写端

        exit(0);
    } else {
        // 父进程：向子进程发送消息
        if (write(fd[1], "ping", 4) < 0) {
            fprintf(2, "write failed\n");
            exit(1);
        }
        close(fd[1]); // 关闭写端

        // 等待子进程完成
        wait(0);

        // 读取子进程发送的消息
        if (read(fd[0], buf, 4) < 0) {
            fprintf(2, "read failed\n");
            exit(1);
        }
        buf[4] = '\0'; // 添加空字符
        printf("%d: received %s\n", getpid(), buf);
        close(fd[0]); // 关闭读端

        exit(0);
    }
}
