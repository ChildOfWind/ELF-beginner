#!/bin/bash

echo "========================================"
echo "GDB 对比延迟绑定和立即绑定"
echo "========================================"
echo ""

echo "----------------------------------------"
echo "1. 延迟绑定模式 (main_lazy)"
echo "----------------------------------------"
echo ""

cat > /tmp/gdb_lazy.txt << 'EOF'
set pagination off
set print pretty on

echo \n=== 查看 func_a 的 GOT 表项（程序启动时）===\n
break main
run
# 假设 func_a 的 GOT 在 0x404000，需根据实际调整
info symbol 0x404000
x/gx 0x404000

echo \n=== 单步到第一次调用 func_a ===\n
break func_a
continue
echo \n进入 func_a，此时 GOT 已更新\n
x/gx 0x404000

quit
EOF

echo "使用 GDB 观察延迟绑定："
echo "  gdb -batch -x /tmp/gdb_lazy.txt ./main_lazy"
echo ""

echo "----------------------------------------"
echo "2. 立即绑定模式 (main_now)"
echo "----------------------------------------"
echo ""

cat > /tmp/gdb_now.txt << 'EOF'
set pagination off
set print pretty on

echo \n=== 查看 func_a 的 GOT 表项（程序启动时）===\n
break main
run
# 立即绑定模式下，GOT 在程序启动时就已填充
info symbol 0x404000
x/gx 0x404000
echo \n注意：立即绑定模式下，GOT 已经是实际地址\n

quit
EOF

echo "使用 GDB 观察立即绑定："
echo "  gdb -batch -x /tmp/gdb_now.txt ./main_now"
echo ""

echo "----------------------------------------"
echo "手动 GDB 调试步骤"
echo "----------------------------------------"
echo ""
echo "延迟绑定调试："
echo "  gdb ./main_lazy"
echo "  (gdb) break main"
echo "  (gdb) run"
echo "  (gdb) disassemble func_a@plt"
echo "  (gdb) x/3i 0x401080          # 查看 PLT 代码"
echo "  (gdb) x/gx 0x404000          # 查看 GOT（调用前）"
echo "  (gdb) break func_a"
echo "  (gdb) continue"
echo "  (gdb) x/gx 0x404000          # 查看 GOT（调用后）"
echo ""
echo "立即绑定调试："
echo "  gdb ./main_now"
echo "  (gdb) break main"
echo "  (gdb) run"
echo "  (gdb) x/gx 0x404000          # 已经是实际地址"
echo ""

# 清理
rm -f /tmp/gdb_lazy.txt /tmp/gdb_now.txt

echo "========================================"

