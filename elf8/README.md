# ELF 辅助向量 (Auxiliary Vector) 研究

这个目录包含用于研究 ELF 辅助向量 (auxv) 的示例程序。

## 什么是辅助向量 (Auxiliary Vector)?

辅助向量是内核传递给用户空间程序的一组键值对，位于进程栈的顶部。它包含了程序启动时需要的重要信息，如：

- 程序头表地址和大小
- 系统页面大小
- 程序入口点
- 用户ID和组ID
- 硬件能力
- 随机数（用于ASLR等安全功能）
- vDSO地址
- 等等...

## 栈布局

程序启动时，栈的布局如下：

```
高地址
    +-----------------+
    | 环境变量字符串   |
    | 参数字符串      |
    +-----------------+
    | NULL           |
    +-----------------+
    | auxv[n]        |  <- 辅助向量数组
    | ...            |
    | auxv[1]        |
    | auxv[0]        |
    +-----------------+
    | NULL           |
    +-----------------+
    | envp[n]        |  <- 环境变量指针数组
    | ...            |
    | envp[0]        |
    +-----------------+
    | NULL           |
    +-----------------+
    | argv[argc-1]   |  <- 参数指针数组
    | ...            |
    | argv[0]        |
    +-----------------+
    | argc           |  <- 参数数量
    +-----------------+
低地址 (栈顶)
```

## 文件说明

- `auxv_demo.c` - 综合示例程序，展示三种访问auxv的方法
- `Makefile` - 编译和运行脚本
- `stack_layout.dot` - 栈布局示意图的DOT源文件
- `generate_diagram.sh` - 生成栈布局图的脚本
- `README.md` - 本文档

## 编译和运行

```bash
# 编译
make

# 运行程序
make run

# 使用 GDB 调试
make debug

# 查看 ELF 文件信息
make elf

# 生成栈布局示意图
make diagram
```

## 程序功能

`auxv_demo.c` 展示了三种访问辅助向量的方法：

### 方法1: 从 `/proc/self/auxv` 读取
这是最简单直接的方法，读取内核提供的 auxv 文件。

```c
int fd = open("/proc/self/auxv", O_RDONLY);
ElfW(auxv_t) auxv;
while (read(fd, &auxv, sizeof(auxv)) == sizeof(auxv)) {
    // 处理 auxv 条目
}
```

### 方法2: 使用 `getauxval()` 函数
通过 glibc 提供的函数获取特定的 auxv 值。

```c
unsigned long page_size = getauxval(AT_PAGESZ);
unsigned long entry = getauxval(AT_ENTRY);
```

### 方法3: 直接从栈访问
通过 `main` 函数的 `envp` 参数定位到 auxv 数组。

```c
void access_auxv(char **envp) {
    // 跳过环境变量
    char **env = envp;
    while (*env != NULL) env++;
    env++; // 跳过 NULL
    
    // 现在 env 指向 auxv
    ElfW(auxv_t) *auxv = (ElfW(auxv_t) *)env;
}
```

## 常见的 auxv 类型

| 类型 | 名称 | 描述 |
|------|------|------|
| 0 | AT_NULL | 数组结束标记 |
| 3 | AT_PHDR | 程序头表地址 |
| 4 | AT_PHENT | 程序头表条目大小 |
| 5 | AT_PHNUM | 程序头表条目数量 |
| 6 | AT_PAGESZ | 系统页面大小 |
| 7 | AT_BASE | 解释器基地址 |
| 9 | AT_ENTRY | 程序入口点地址 |
| 11 | AT_UID | 真实用户ID |
| 12 | AT_EUID | 有效用户ID |
| 13 | AT_GID | 真实组ID |
| 14 | AT_EGID | 有效组ID |
| 15 | AT_PLATFORM | 平台字符串 |
| 16 | AT_HWCAP | 硬件能力位 |
| 17 | AT_CLKTCK | 时钟频率 |
| 23 | AT_SECURE | 是否安全模式 |
| 25 | AT_RANDOM | 16字节随机数地址 |
| 26 | AT_HWCAP2 | 硬件能力位(扩展) |
| 31 | AT_EXECFN | 可执行文件名 |
| 33 | AT_SYSINFO_EHDR | vDSO地址 |

## 使用 GDB 调试

```bash
# 启动 GDB
gdb ./auxv_demo

# 在 main 设置断点
(gdb) break main

# 运行程序
(gdb) run

# 查看 auxv
(gdb) info auxv

# 查看栈内容（x86_64）
(gdb) x/20xg $rsp

# 查看 argc
(gdb) print argc

# 查看 argv
(gdb) print argv
(gdb) x/s argv[0]

# 查看 envp
(gdb) print envp
(gdb) x/10xg envp

# 定位 auxv
# 先找到 envp 结尾的 NULL
# 然后查看下一个地址就是 auxv

# 单步执行
(gdb) next

# 继续执行
(gdb) continue
```

## 实验建议

1. **观察内存布局**: 运行程序，注意 auxv 在栈上的实际位置
2. **对比三种方法**: 验证三种方法获取的 auxv 数据是否一致
3. **使用 GDB**: 在 GDB 中单步执行，观察栈的变化
4. **修改程序**: 尝试添加新的 auxv 类型，或修改显示格式
5. **查看 /proc**: 运行程序时，在另一个终端查看 `/proc/<pid>/auxv`

## 进阶研究

- 研究动态链接器如何使用 auxv
- 分析 vDSO (AT_SYSINFO_EHDR) 的内容
- 研究 ASLR 与 AT_RANDOM 的关系
- 查看不同架构下的 auxv 差异

## 相关资源

- Linux man page: `man 3 getauxval`
- `/usr/include/elf.h` - auxv 类型定义
- Linux 内核源码: `fs/binfmt_elf.c` - auxv 的创建过程
- glibc 源码: `elf/dl-support.c` - auxv 的使用

## 参考命令

```bash
# 查看当前进程的 auxv (需要程序运行时)
cat /proc/self/auxv | od -t x8

# 查看 ELF 文件的程序头表
readelf -l auxv_demo

# 查看 ELF 文件头
readelf -h auxv_demo

# 查看符号表
readelf -s auxv_demo

# 反汇编
objdump -d auxv_demo
```

