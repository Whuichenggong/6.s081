#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define MAX_ARGS 512

int main(int argc, char *argv[]) {
    char buf[512];
    char *args[MAX_ARGS];
    int n, i;

    if (argc < 2) {
        fprintf(2, "Usage: xargs <command> [args...]\n");
        exit(1);
    }

    // 初始化参数列表
    for (i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }

    while ((n = read(0, buf, sizeof(buf))) > 0) {
        if (n < 0) {
            fprintf(2, "xargs: read error\n");
            exit(1);
        }

        // 将输入数据解析为参数
        char *p = buf;
        while (p < buf + n) {
            // 跳过空白字符
            while (p < buf + n && (*p == ' ' || *p == '\n')) {
                p++;
            }

            // 如果遇到非空白字符，将其作为参数
            if (p < buf + n && *p != '\0') {
                if (argc >= MAX_ARGS) {
                    fprintf(2, "xargs: too many arguments\n");
                    exit(1);
                }
                args[argc - 1] = p;
                argc++;
                while (p < buf + n && *p != '\0' && *p != ' ' && *p != '\n') {
                    p++;
                }
                *p++ = '\0'; // 终止当前参数
            }
        }

        // 添加 NULL 终止符
        args[argc - 1] = 0;

        // 执行命令
        if (fork() == 0) {
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        } else {
            wait(0);
        }
    }

    exit(0);
}
