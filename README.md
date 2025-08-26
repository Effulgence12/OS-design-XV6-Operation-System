# 同济大学 2025 年暑期操作系统课程设计——xv6 操作系统实验

本仓库包含 MIT 6.828 操作系统课程（2020 年）的 xv6 实验的全部源代码，xv6 是一个基于 Unix v6 的教学用操作系统，用于学习操作系统的核心概念。

## 实验内容

实现了课程中的所有实验，包括：

- Lab1: Utilities 实用程序
- Lab2: System Calls 系统调用
- Lab3: Page Tables 页表
- Lab4: Traps 陷阱
- Lab5: Lazy allocation 懒页分配
- Lab6: Copy-on-Write Fork for xv6 写时复制 Fork
- Lab7: Multithreading 多线程
- Lab8: Locks 锁
- Lab9: File system 文件系统
- Lab10: Mmap 内存映射
- Lab11: Networking 网络

## 编译与运行

不同实验代码存储在不同分支内，切换分支以查看实验源代码

```bash
# 编译xv6
git checkout <分支名/实验名>

# 运行xv6（带QEMU模拟器）
make qemu

# 运行测试
make grade
```
