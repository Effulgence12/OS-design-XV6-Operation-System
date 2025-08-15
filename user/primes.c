#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int subProcess(int *oldFd)
{
    close(oldFd[1]); // 关闭原管道写端
    int fd[2];
    int prime;
    int num;
    if (read(oldFd[0], &prime, 4)) // 若能从原管道读到数据
    {
        printf("prime %d\n", prime); // 第一个数据为质数，进行输出
        pipe(fd);                    // 创建新管道和子进程
        if (fork() == 0)             // 子进程
            subProcess(fd);          // 递归调用
        else                         // 父进程
        {
            close(fd[0]);                   // 关闭新管道读端
            while (read(oldFd[0], &num, 4)) // 从原管道读取数据
            {
                if (num % prime != 0) // 不能被当前质数整除的数写入新管道
                    write(fd[1], &num, 4);
            }
            close(oldFd[0]); // 原管道数据读取完毕，关闭读端
            close(fd[1]);    // 关闭新管道写端
            wait((int *)0);  // 等待子进程结束
        }
    }
    else
        close(oldFd[0]); // 无法从原管道读取数据时，直接关闭读端
    exit(0);
}

int main()
{
    int fd[2];
    pipe(fd);
    if (fork() == 0) // 子进程
        subProcess(fd);
    else // 父进程
    {
        close(fd[0]);
        for (int i = 2; i <= 35; ++i) // 遍历2~35的整数，写入管道写端
            write(fd[1], &i, 4);
        close(fd[1]); // 数据写入完成，关闭管道写端并等待子进程结束
        wait((int *)0);
    }
    exit(0);
}
