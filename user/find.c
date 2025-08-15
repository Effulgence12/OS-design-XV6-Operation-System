#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 提取路径中的文件名部分
char *pfilename(char *path)
{
    char *p;
    // 从路径末尾向前查找最后一个'/'
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++; // 指向文件名的第一个字符
    return p;
}

// 递归查找文件
int find(char *path, char *filename)
{
    int fd;            // 目录文件描述符
    char buf[512], *p; // 路径缓冲区及指针
    struct stat st;    // 文件状态结构体
    struct dirent de;  // 目录项结构体

    // 尝试打开路径
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "open fail%s\n", path);
        exit(1);
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "fstat fail%s\n", path);
        close(fd);
        exit(1);
    }

    switch (st.type)
    {
    case T_FILE: // 常规文件
        // 比较文件名与目标文件名，匹配则输出路径
        if (0 == strcmp(pfilename(path), filename))
            fprintf(1, "%s\n", path);
        break;

    case T_DIR: // 目录
        strcpy(buf, path);       // 复制路径到缓冲区
        p = buf + strlen(buf);   // 移动指针到路径末尾
        *p++ = '/';              // 添加路径分隔符

        // 遍历目录项
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0) // 跳过无效条目
                continue;
            // 跳过当前目录(.)和上级目录(..)
            if (0 == strcmp(".", de.name) || 0 == strcmp("..", de.name))
                continue;

            // 拼接完整路径
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0; // 确保字符串结束符

            // 获取文件状态，失败则输出提示并继续
            if (stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }

            find(buf, filename); // 递归查找子目录
        }
        break;
    }

    close(fd); // 关闭文件描述符
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3) // 检查参数数量
    {
        fprintf(2, "not enough arguments\n");
        exit(1);
    }
    find(argv[1], argv[2]); // 调用查找函数
    exit(0);
}
