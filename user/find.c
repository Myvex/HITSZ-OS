#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 提取文件名的函数，返回路径中的最后一个 '/' 之后的部分
char* fmtname(char *path) {
    char *p;
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    return p + 1;
}

// 递归查找函数
void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开当前路径
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取路径状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }


    switch (st.type) {

        case T_FILE:// 如果是文件，检查文件名是否匹配
            if (strcmp(fmtname(path), target) == 0) {
                printf("%s\n", path);
            }
            break;

        case T_DIR://是目录
            if (strcmp(fmtname(path), target) == 0) {
                printf("%s\n", path);
            }
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
                printf("find: path too long\n");
            }

            strcpy(buf, path);  // 复制当前路径
            p = buf + strlen(buf);
            *p++ = '/';

            // 读取当前目录中的每个文件或子目录
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0)  // 跳过空目录项
                    continue;
                // 跳过 "." 和 ".."
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                // 构建子路径
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                // 获取子路径的状态
                if (stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                // 递归进入子目录或检查文件
                find(buf, target);
            }
        break;
        }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <name>\n");
        exit(1);
    }

    // 从指定路径开始查找目标文件或目录
    find(argv[1], argv[2]);

    exit(0);
}