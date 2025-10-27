#!/bin/bash

# 动态链接分析脚本 - 详细分析PLT和GOT

echo "========================================"
echo "动态链接机制分析工具"
echo "========================================"
echo ""

if [ ! -f "./main" ]; then
    echo "错误: main 程序不存在，请先运行 make"
    exit 1
fi

# 1. PLT (Procedure Linkage Table) 分析
echo "1. PLT (过程链接表) 分析"
echo "----------------------------------------"
echo "PLT是一段桩代码，用于延迟绑定函数地址"
echo ""
objdump -M intel -d -j .plt main | grep -A 3 "<add@plt>:"
objdump -M intel -d -j .plt main | grep -A 3 "<multiply@plt>:"
objdump -M intel -d -j .plt main | grep -A 3 "<print_message@plt>:"
echo ""

# 2. GOT (Global Offset Table) 分析
echo "2. GOT (全局偏移表) 分析"
echo "----------------------------------------"
echo "GOT存储动态符号的实际地址"
echo ""
readelf -r main | grep -E "(add|multiply|print_message|shared_variable)"
echo ""

# 3. 动态符号表
echo "3. 动态符号表"
echo "----------------------------------------"
readelf --dyn-syms main | grep -E "(add|multiply|print_message|shared_variable)"
echo ""

# 4. 反汇编main函数，查看函数调用
echo "4. main函数中的PLT调用"
echo "----------------------------------------"
echo "查看main函数如何通过PLT调用动态库函数"
echo ""
objdump -M intel -d main | grep -A 2 "call.*@plt"
echo ""

# 5. 使用GDB跟踪延迟绑定
echo "5. 使用GDB跟踪延迟绑定过程"
echo "----------------------------------------"
echo "可以使用以下GDB命令观察延迟绑定:"
echo ""
echo "  gdb ./main"
echo "  (gdb) b add"
echo "  (gdb) r"
echo "  (gdb) x/3i \$pc"
echo "  (gdb) disassemble add@plt"
echo "  (gdb) x/gx &add  # 查看GOT表项"
echo ""

# 6. 动态库依赖
echo "6. 动态库依赖关系"
echo "----------------------------------------"
ldd main
echo ""

# 7. 重定位信息详细分析
echo "7. 重定位信息详细分析"
echo "----------------------------------------"
readelf -r main
echo ""

echo "========================================"
echo "提示: 运行 'make run' 来执行程序"
echo "      运行程序前后可以用GDB观察GOT表的变化"
echo "========================================"

