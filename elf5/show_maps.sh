#!/bin/bash
#
# 显示进程内存布局的辅助脚本
#

if [ -z "$1" ]; then
    echo "用法: $0 <PID>"
    echo "示例: $0 12345"
    exit 1
fi

PID=$1

if [ ! -d "/proc/$PID" ]; then
    echo "错误: 进程 $PID 不存在"
    exit 1
fi

echo "=========================================="
echo "进程 $PID 的内存布局 (/proc/$PID/maps)"
echo "=========================================="
echo ""

# 显示完整的 maps
cat /proc/$PID/maps

echo ""
echo "=========================================="
echo "关键内存区域摘要"
echo "=========================================="
echo ""

# 代码段
echo "【代码段 (.text)】"
cat /proc/$PID/maps | grep "r-xp" | grep "memory_layout"
echo ""

# 数据段
echo "【数据段 (.data/.bss)】"
cat /proc/$PID/maps | grep "rw-p" | grep "memory_layout"
echo ""

# 堆区
echo "【堆区 (heap)】"
cat /proc/$PID/maps | grep "heap"
echo ""

# 栈区
echo "【栈区 (stack)】"
cat /proc/$PID/maps | grep "stack"
echo ""

# 动态链接库
echo "【动态链接库】"
cat /proc/$PID/maps | grep "\.so"
echo ""

# 匿名映射
echo "【匿名映射区】"
cat /proc/$PID/maps | grep "rw-p" | grep -v "memory_layout" | grep -v "\.so" | grep -v "heap" | grep -v "stack"
echo ""

echo "=========================================="
echo "详细说明"
echo "=========================================="
echo "内存权限标识:"
echo "  r = 可读 (read)"
echo "  w = 可写 (write)"
echo "  x = 可执行 (execute)"
echo "  p = 私有映射 (private)"
echo "  s = 共享映射 (shared)"
echo ""
echo "典型布局（从低地址到高地址）:"
echo "  1. 代码段 (.text)     - r-xp 可读可执行"
echo "  2. 只读数据 (.rodata) - r--p 只读"
echo "  3. 数据段 (.data)     - rw-p 可读可写"
echo "  4. BSS段 (.bss)       - rw-p 可读可写"
echo "  5. 堆 (heap)          - rw-p 向高地址增长"
echo "  6. 内存映射区         - rw-p 共享库等"
echo "  7. 栈 (stack)         - rw-p 向低地址增长"
echo ""

