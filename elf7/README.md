# ELF 动态链接：延迟绑定 vs 立即绑定

本项目深入研究动态链接的两种模式：**延迟绑定（Lazy Binding）** 和 **立即绑定（Immediate Binding）**，展示 PLT/GOT 机制的工作原理。

## 文件说明

- `libtest.c/h`: 简单的测试动态库
- `main.c`: 调用动态库函数的主程序
- `main_lazy`: 延迟绑定版本（默认模式）
- `main_now`: 立即绑定版本（-Wl,-z,now）
- `compare.sh`: 对比两种模式的差异
- `analyze_binding.sh`: 详细分析 PLT/GOT 机制
- `gdb_compare.sh`: GDB 调试指南

## 快速开始

```bash
# 编译
make

# 运行两个版本
make run

# 对比分析
make compare

# 详细分析
make analyze
```

## 核心概念

### 1. 延迟绑定（Lazy Binding）

**默认模式**，函数地址在第一次调用时才解析。

#### 优点：
- ✓ 程序启动快
- ✓ 节省内存（未使用的函数不加载）
- ✓ 减少不必要的符号解析

#### 缺点：
- ✗ 首次调用有性能开销
- ✗ 运行时才发现符号缺失
- ✗ 安全风险（GOT 可被劫持）

#### 工作流程：

```
第一次调用 func_a():
  main → func_a@plt → GOT[func_a] → PLT[0] 
       → _dl_runtime_resolve → 解析符号
       → 更新 GOT[func_a] → func_a

第二次调用 func_a():
  main → func_a@plt → GOT[func_a] → func_a (直接跳转)
```

### 2. 立即绑定（Immediate Binding）

**显式启用**，所有函数地址在程序启动时解析。

#### 优点：
- ✓ 运行时性能稳定
- ✓ 启动时发现符号缺失
- ✓ 更安全（GOT 只写一次）

#### 缺点：
- ✗ 程序启动慢
- ✗ 内存占用高
- ✗ 解析不需要的符号

#### 工作流程：

```
程序启动时:
  _start → _dl_init → 解析所有符号 → 填充所有 GOT 表项

调用 func_a():
  main → func_a@plt → GOT[func_a] → func_a (直接跳转)
```

## PLT/GOT 机制详解

### PLT (Procedure Linkage Table)

PLT 是一段**桩代码**，每个外部函数都有对应的 PLT 条目：

```assembly
<func_a@plt>:
  jmp    QWORD PTR [rip+0x2f52]   # 跳转到 GOT[func_a]
  push   0x0                       # 重定位索引
  jmp    401020 <.plt>             # 跳转到 PLT[0] resolver
```

### GOT (Global Offset Table)

GOT 是一个**函数指针表**，存储外部符号的实际地址：

| 模式     | GOT 初始值              | GOT 最终值        |
|----------|-------------------------|-------------------|
| 延迟绑定 | PLT[1] 地址 (resolver) | 实际函数地址      |
| 立即绑定 | 实际函数地址            | 实际函数地址      |

## 两种模式对比

### 编译差异

```bash
# 延迟绑定（默认）
gcc -o main_lazy main.c -L. -ltest

# 立即绑定
gcc -o main_now main.c -L. -ltest -Wl,-z,now
```

### 二进制差异

```bash
# 查看动态段标志
readelf -d main_lazy | grep FLAGS    # 无 BIND_NOW
readelf -d main_now  | grep FLAGS    # 有 BIND_NOW 或 DF_BIND_NOW
```

### 运行时差异

```bash
# 运行时启用立即绑定
LD_BIND_NOW=1 ./main_lazy

# 运行时禁用延迟绑定（如果支持）
LD_BIND_NOT=1 ./main_now
```

## 使用 GDB 观察

### 观察延迟绑定

```bash
gdb ./main_lazy
(gdb) break main
(gdb) run
(gdb) disassemble func_a@plt          # 查看 PLT 代码
(gdb) x/gx 'func_a@got.plt'           # GOT 初始值（指向 PLT）
(gdb) break func_a
(gdb) continue
(gdb) x/gx 'func_a@got.plt'           # GOT 已更新（实际地址）
```

### 观察立即绑定

```bash
gdb ./main_now
(gdb) break main
(gdb) run
(gdb) x/gx 'func_a@got.plt'           # 启动时就是实际地址
```

## 性能对比

```bash
# 测试启动时间
time ./main_lazy
time ./main_now

# 使用 strace 观察系统调用
strace -e openat,mmap ./main_lazy 2>&1 | grep libtest
strace -e openat,mmap ./main_now  2>&1 | grep libtest
```

## 安全考虑

### GOT 劫持攻击

延迟绑定模式下，GOT 在运行时可写，可能被攻击者修改：

```
攻击者 → 修改 GOT[func_a] → 恶意代码
```

### 防护措施

1. **使用立即绑定**: `-Wl,-z,now`
2. **GOT 只读**: `-Wl,-z,relro` (完全 RELRO)
3. **组合使用**:
   ```bash
   gcc -Wl,-z,now,-z,relro -o main_secure main.c -L. -ltest
   ```

## 实际应用建议

### 何时使用延迟绑定
- 大型应用程序
- 插件系统
- 快速启动优先

### 何时使用立即绑定
- 安全关键应用
- 实时系统
- 服务器程序（守护进程）

## 深入研究

```bash
# 查看完整的 PLT/GOT
objdump -M intel -d -j .plt main_lazy
objdump -M intel -d -j .plt.got main_lazy

# 查看重定位信息
readelf -r main_lazy

# 查看动态符号
readelf --dyn-syms main_lazy

# 使用 ltrace 跟踪库函数调用
ltrace ./main_lazy

# 使用 LD_DEBUG 查看动态链接过程
LD_DEBUG=bindings ./main_lazy
LD_DEBUG=bindings ./main_now
```

## 参考资料

- ELF Specification
- System V ABI
- GNU ld 文档
- `man ld.so`

