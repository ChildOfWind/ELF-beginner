#!/bin/bash

echo ""
echo "========================================"
echo "PLT/GOT 机制详细分析"
echo "========================================"

if [ ! -f "./main_lazy" ] || [ ! -f "./main_now" ]; then
    echo "错误: 请先运行 make"
    exit 1
fi

echo ""
echo "----------------------------------------"
echo "一、延迟绑定模式 (Lazy Binding)"
echo "----------------------------------------"
echo ""
echo "1. PLT 条目结构 (以 func_a 为例):"
objdump -M intel -d main_lazy | grep -A 5 "<func_a@plt>:"
echo ""

echo "2. PLT 工作流程:"
echo "   第一次调用 func_a@plt:"
echo "   ① jmp [GOT表项]     # GOT初始指向下一条指令"
echo "   ② push 重定位索引   # 压入函数索引"
echo "   ③ jmp PLT[0]        # 跳转到动态链接器"
echo "   ④ 动态链接器解析符号"
echo "   ⑤ 更新GOT表项为实际地址"
echo "   ⑥ 跳转到真实函数"
echo ""
echo "   后续调用:"
echo "   ① jmp [GOT表项]     # 直接跳转到实际地址"
echo ""

echo "3. 重定位类型:"
readelf -r main_lazy | grep -E "(func_a|func_b|func_c|calculate)" | head -4
echo ""

echo "4. 动态段信息:"
readelf -d main_lazy | grep -E "(NEEDED|FLAGS|BIND)"
echo ""

echo ""
echo "----------------------------------------"
echo "二、立即绑定模式 (Immediate Binding)"
echo "----------------------------------------"
echo ""
echo "1. PLT 条目结构 (以 func_a 为例):"
objdump -M intel -d main_now | grep -A 5 "<func_a@plt>:"
echo ""

echo "2. 立即绑定特点:"
echo "   程序启动时:"
echo "   ① 动态链接器立即解析所有符号"
echo "   ② GOT表直接填充实际函数地址"
echo "   ③ 没有延迟解析的开销"
echo ""
echo "   调用函数时:"
echo "   ① jmp [GOT表项]     # 直接跳转（GOT已是实际地址）"
echo ""

echo "3. 重定位类型:"
readelf -r main_now | grep -E "(func_a|func_b|func_c|calculate)" | head -4
echo ""

echo "4. 动态段信息 (注意 BIND_NOW 标志):"
readelf -d main_now | grep -E "(NEEDED|FLAGS|BIND)"
echo ""

echo ""
echo "----------------------------------------"
echo "三、两种模式对比总结"
echo "----------------------------------------"
echo ""
echo "┌─────────────┬──────────────────┬──────────────────┐"
echo "│   特性      │   延迟绑定       │   立即绑定       │"
echo "├─────────────┼──────────────────┼──────────────────┤"
echo "│ 启动速度    │   快             │   慢             │"
echo "│ 首次调用    │   慢(需解析)     │   快(已解析)     │"
echo "│ 后续调用    │   快             │   快             │"
echo "│ 内存占用    │   按需加载       │   全部加载       │"
echo "│ 安全性      │   较低           │   较高           │"
echo "│ 默认模式    │   是             │   否             │"
echo "└─────────────┴──────────────────┴──────────────────┘"
echo ""

echo "使用场景:"
echo "  • 延迟绑定: 快速启动，部分函数可能不被调用"
echo "  • 立即绑定: 安全关键应用，需要避免运行时解析"
echo ""

echo "启用立即绑定的方法:"
echo "  1. 编译时: gcc -Wl,-z,now ..."
echo "  2. 环境变量: LD_BIND_NOW=1 ./program"
echo ""

