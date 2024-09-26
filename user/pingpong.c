#include "kernel/types.h"
#include "user.h"

int main(int argc,char* argv[])
{
    int c2f[2];// 子进程 -> 父进程管道
    int f2c[2];// 父进程 -> 子进程管道

    // 创建两个管道
    pipe(c2f);
    pipe(f2c);

    int parent_pid = getpid();
    int child_pid = fork();  // 创建子进程
    
    //printf("parent:%d,child:%d\n",parent_pid,child_pid);

    if(child_pid>0)//父进程
    {
        close(f2c[0]);//关闭父进程的读端
        close(c2f[1]);//关闭子进程的写端

        char *msg = "ping";
        write(f2c[1], msg, 4);  // 向子进程发送 "ping"
        close(f2c[1]);  // 关闭父进程写端

        // 父进程读取子进程的响应
        char buf[5];
        read(c2f[0], buf, 4);  // 从子进程读取 "pong"
        buf[4] = '\0';  // 确保字符串以 NULL 结尾
        printf("%d: received %s from pid %d\n", parent_pid, buf, child_pid);
        close(c2f[0]);  // 关闭子进程读取端

        wait(0);  // 等待子进程结束
    }
    else if(child_pid==0)
    {
        close(f2c[1]);//关闭父进程写端
        close(c2f[0]);//关闭子进程读端

        char buf[5];
        read(f2c[0], buf, 4);  // 从父进程读取 "ping"
        buf[4] = '\0';  // 确保字符串以 NULL 结尾
        printf("%d: received %s from pid %d\n", getpid(), buf, parent_pid);

        // 子进程向父进程发送 "pong"
        char *msg = "pong";
        write(c2f[1], msg, 4);
        close(f2c[0]);  // 关闭读取端
        close(c2f[1]);  // 关闭写端
    }

	exit(0);
}