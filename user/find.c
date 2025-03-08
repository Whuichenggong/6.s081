#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    //memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

void
find(char* dir_name, char* file_name)
{
    char buf[256], *p;
    int fd;
    struct stat st;
    struct dirent de;

    if((fd = open(dir_name, 0)) < 0) {
        fprintf(2, "find: cannot open dir %s\n", dir_name);
        return;
    }

    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat dir %s\n", dir_name);
        close(fd);
        return;
    }

    // 如果是文件，则直接输出
    if(st.type == T_FILE) {
        if(!strcmp(fmtname(dir_name), file_name)) {
            printf("%s\n", dir_name);
        }
        close(fd);
        return;
    }

    // 如果是目录，则遍历目录下的文件
    if(st.type == T_DIR) {
        if(strlen(dir_name) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("find: path too long\n");
            close(fd);
            return;
        }

        strcpy(buf, dir_name);
        p = buf + strlen(buf); // 定位指针
        *p++ = '/';

        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0) {
                continue;
            }

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            struct stat st_tmp;
            if(stat(buf, &st_tmp) < 0) {
                continue;
            }

            if(st_tmp.type == T_FILE) { // 如果是普通文件
                if(!strcmp(de.name, file_name)) { // 找到文件
                    printf("%s\n", buf); // 打印文件的相对路径
                }
            }

            if(st_tmp.type == T_DIR) { // 如果是目录
                // 禁止遍历 . 和 .. 这两个目录
                if((!strcmp(de.name, ".")) || (!strcmp(de.name, ".."))) {
                    continue;
                }
                find(buf, file_name); // 递归搜索
            }
        }
    }

    close(fd);
    return;
}

int
main(int argc, char* argv[])
{
    if(argc < 3) {
        fprintf(2, "Usage: find <directory> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}
