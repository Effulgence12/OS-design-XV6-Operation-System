#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
    char *args[MAXARG]; // 用于存储执行命令的参数
    int p;
    // 将xargs自身的命令行参数存入args数组
    for (p = 0; p < argc; p++)
        args[p] = argv[p];
    
    char buf[256]; // 用于存储从标准输入读取的一行内容
    while (1) // 循环读取标准输入的每一行
    {
        int i = 0;
        // 读取一行内容（以换行符'\n'为结束标志）
        while ((read(0, buf + i, sizeof(char)) != 0) && buf[i] != '\n')
            i++;
        
        if (i == 0) // 若未读取到内容，说明已读完所有行，退出循环
            break;
        
        buf[i] = 0;      // 在字符串末尾添加结束符（exec函数要求）
        args[p] = buf;   // 将读取到的行内容附加到参数列表后
        args[p + 1] = 0; // 标记参数列表结束
        
        if (fork() == 0) // 创建子进程执行命令
        {
            // 执行args[1]指定的命令，传入args+1作为参数
            exec(args[1], args + 1);
            printf("exec err\n"); // 若exec执行失败，打印错误信息
        }
        else
        {
            wait((void *)0); // 父进程等待子进程执行完毕
        }
    }
    exit(0);
}