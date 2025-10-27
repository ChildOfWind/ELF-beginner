#!/bin/bash
#
# 持续监控进程内存变化
#

if [ -z "$1" ]; then
    echo "用法: $0 <PID>"
    exit 1
fi

PID=$1

if [ ! -d "/proc/$PID" ]; then
    echo "错误: 进程 $PID 不存在"
    exit 1
fi

echo "监控进程 $PID 的内存布局（每3秒刷新一次）"
echo "按 Ctrl+C 退出"
echo ""

while true; do
    clear
    echo "=========================================="
    echo "时间: $(date '+%Y-%m-%d %H:%M:%S')"
    echo "进程: $PID"
    echo "=========================================="
    echo ""
    
    if [ ! -d "/proc/$PID" ]; then
        echo "进程 $PID 已退出"
        exit 0
    fi
    
    # 内存统计
    echo "【内存使用统计】"
    cat /proc/$PID/status | grep -E "VmSize|VmRSS|VmData|VmStk|VmExe"
    echo ""
    
    # 关键内存区域
    echo "【关键内存区域】"
    echo ""
    echo "可执行文件本身:"
    cat /proc/$PID/maps | grep "r-xp" | grep "memory_layout" | head -1
    echo ""
    echo "堆区:"
    cat /proc/$PID/maps | grep "heap"
    echo ""
    echo "栈区:"
    cat /proc/$PID/maps | grep "stack"
    echo ""
    
    sleep 3
done

