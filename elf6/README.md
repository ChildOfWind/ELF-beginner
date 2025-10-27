# 动态链接机制演示

本示例演示Linux下的动态链接机制，包括PLT、GOT和延迟加载（Lazy Binding）。

## 文件说明

- `mylib.h` / `mylib.c`: 简单的动态库，提供函数和全局变量
- `main.c`: 主程序，调用动态库中的函数和访问变量
- `Makefile`: 编译脚本
- `analyze_plt_got.sh`: PLT/GOT分析脚本

## 核心概念

### 1. PLT (Procedure Linkage Table - 过程链接表)
- 位于 `.plt` 段
- 包含一段桩代码，用于跳转到实际的函数地址
- 每个外部函数都有对应的PLT条目

### 2. GOT (Global Offset Table - 全局偏移表)
- 位于 `.got.plt` 段
- 存储外部符号（函数/变量）的实际地址
- 运行时由动态链接器填充

### 3. 延迟绑定 (Lazy Binding)
- 第一次调用函数时才解析地址
- 通过PLT和GOT协作实现
- 提高程序启动速度

## 编译和运行

```bash
# 编译
make

# 运行程序
make run

# 分析PLT和GOT
make analyze

# 详细分析
./analyze_plt_got.sh
```

## 动态链接流程

### 第一次调用函数时（延迟绑定）：

1. 调用 `add@plt`
2. PLT跳转到GOT表项（初始指向PLT下一条指令）
3. 压入重定位索引
4. 跳转到 `_dl_runtime_resolve`
5. 动态链接器解析符号，更新GOT
6. 跳转到真实的 `add` 函数

### 第二次调用同一函数时：

1. 调用 `add@plt`
2. PLT跳转到GOT表项（已更新为实际地址）
3. 直接执行 `add` 函数

## 使用GDB观察延迟绑定

```bash
gdb ./main
(gdb) break main
(gdb) run
(gdb) disassemble add@plt          # 查看PLT代码
(gdb) x/gx 'add@got.plt'           # 查看GOT表项（首次）
(gdb) break add
(gdb) continue
(gdb) x/gx 'add@got.plt'           # 查看GOT表项（已解析）
```

## 关键观察点

1. **PLT条目结构**: 每个PLT条目约16字节，包含跳转指令
2. **GOT初始值**: 指向PLT中的resolver代码
3. **GOT更新**: 第一次调用后，GOT被更新为实际函数地址
4. **全局变量**: 通过GOT访问，没有延迟绑定

## 禁用延迟绑定

如果想在程序启动时就解析所有符号（立即绑定），可以：

```bash
# 方法1: 设置环境变量
LD_BIND_NOW=1 ./main

# 方法2: 编译时指定
gcc -Wl,-z,now -o main main.c -L. -lmylib
```

