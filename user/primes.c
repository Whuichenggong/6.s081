#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void create_child(int parent_fd[]) {
    int p;
    // 读取第一个数字，它一定是质数
    if (read(parent_fd[0], &p, sizeof(p)) == 0) {
        close(parent_fd[0]);
        exit(0);
    }
    printf("prime %d\n", p); // 立即打印当前质数

    int child_fd[2];
    pipe(child_fd);

    if (fork() == 0) {
        // 子进程：关闭不必要的文件描述符，递归处理
        close(parent_fd[0]);
        close(child_fd[1]);
        create_child(child_fd);
        exit(0);
    } else {
        // 父进程：过滤并传递剩余数字
        close(child_fd[0]);
        int num;
        while (read(parent_fd[0], &num, sizeof(num)) > 0) {
            if (num % p != 0) {
                write(child_fd[1], &num, sizeof(num));
            }
        }
        close(parent_fd[0]);
        close(child_fd[1]);
        wait(0); // 等待子进程结束
    }
}

int main() {
    int initial_fd[2];
    pipe(initial_fd);

    for (int i = 2; i <= 35; i++) {
        write(initial_fd[1], &i, sizeof(i));
    }
    close(initial_fd[1]);

    create_child(initial_fd);

    // 确保所有子进程结束
    while (wait(0) > 0);
    exit(0);
}
