# Makefile for studying process memory layout
# 用于研究进程内存布局

CC = gcc
CFLAGS = -Wall -g -no-pie -fno-pie
TARGET = memory_layout

.PHONY: all run clean show help

# 默认目标
all: $(TARGET) scripts
	@echo "=== 编译完成 ==="
	@echo "运行 'make run' 启动进程"
	@echo "运行 'make help' 查看使用说明"

# 编译程序
$(TARGET): memory_layout.c
	@echo "=== 编译 memory_layout.c ==="
	$(CC) $(CFLAGS) -o $(TARGET) memory_layout.c

# 确保脚本可执行
scripts:
	@chmod +x show_maps.sh analyze_memory.sh 2>/dev/null || true

# 运行程序
run: $(TARGET)
	@echo "=== 启动进程（保持运行状态）==="
	@echo "程序将持续运行，在另一个终端执行:"
	@echo "  1. 查看内存布局: make show"
	@echo "  2. 或直接使用 PID: cat /proc/\$$(pgrep memory_layout)/maps"
	@echo ""
	./$(TARGET)

# 在后台运行
run-bg: $(TARGET)
	@echo "=== 在后台启动进程 ==="
	./$(TARGET) &
	@sleep 1
	@PID=$$(pgrep memory_layout); \
	if [ -n "$$PID" ]; then \
		echo "进程已启动，PID: $$PID"; \
		echo "运行 'make show' 查看内存布局"; \
	else \
		echo "启动失败"; \
	fi

# 显示内存布局
show:
	@PID=$$(pgrep memory_layout); \
	if [ -z "$$PID" ]; then \
		echo "错误: memory_layout 进程未运行"; \
		echo "请先运行 'make run' 或 'make run-bg'"; \
		exit 1; \
	fi; \
	echo "找到进程 PID: $$PID"; \
	echo ""; \
	./show_maps.sh $$PID

# 持续监控
monitor:
	@PID=$$(pgrep memory_layout); \
	if [ -z "$$PID" ]; then \
		echo "错误: memory_layout 进程未运行"; \
		exit 1; \
	fi; \
	./analyze_memory.sh $$PID

# 直接查看 maps（简化版）
maps:
	@PID=$$(pgrep memory_layout); \
	if [ -z "$$PID" ]; then \
		echo "错误: memory_layout 进程未运行"; \
		exit 1; \
	fi; \
	cat /proc/$$PID/maps

# 查看进程状态
status:
	@PID=$$(pgrep memory_layout); \
	if [ -z "$$PID" ]; then \
		echo "memory_layout 进程未运行"; \
	else \
		echo "进程运行中，PID: $$PID"; \
		echo ""; \
		cat /proc/$$PID/status | grep -E "Name|Pid|VmSize|VmRSS|VmData|VmStk|VmExe"; \
	fi

# 停止进程
stop:
	@PID=$$(pgrep memory_layout); \
	if [ -n "$$PID" ]; then \
		echo "停止进程 $$PID"; \
		kill $$PID; \
	else \
		echo "没有运行的 memory_layout 进程"; \
	fi

# 查看段信息
segments: $(TARGET)
	@echo "=========================================="
	@echo "=== ELF 段信息 ==="
	@echo "=========================================="
	readelf -S $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "=== 程序头信息 ==="
	@echo "=========================================="
	readelf -l $(TARGET)

# 查看符号表中的全局变量地址
symbols: $(TARGET)
	@echo "=========================================="
	@echo "=== 全局符号地址 ==="
	@echo "=========================================="
	@echo "函数符号:"
	nm -n $(TARGET) | grep " T "
	@echo ""
	@echo "已初始化数据 (.data):"
	nm -n $(TARGET) | grep " D "
	@echo ""
	@echo "未初始化数据 (.bss):"
	nm -n $(TARGET) | grep " B "
	@echo ""
	@echo "只读数据 (.rodata):"
	nm -n $(TARGET) | grep " R "

# 对比：编译时地址 vs 运行时地址
compare:
	@echo "=========================================="
	@echo "=== 编译时地址（从ELF文件）==="
	@echo "=========================================="
	@nm -n $(TARGET) | grep -E "main|initialized_global|uninitialized_global" | head -10
	@echo ""
	@PID=$$(pgrep memory_layout); \
	if [ -n "$$PID" ]; then \
		echo "=========================================="; \
		echo "=== 运行时地址（从进程内存）==="; \
		echo "=========================================="; \
		cat /proc/$$PID/maps | grep memory_layout; \
	else \
		echo "提示: 运行 'make run-bg' 后再执行 'make compare' 可以对比运行时地址"; \
	fi

# 清理
clean:
	rm -f $(TARGET)
	@echo "=== 清理完成 ==="

# 帮助信息
help:
	@echo "=========================================="
	@echo "进程内存布局学习工具"
	@echo "=========================================="
	@echo ""
	@echo "基本用法:"
	@echo "  make          - 编译程序"
	@echo "  make run      - 运行程序（前台，持续运行）"
	@echo "  make run-bg   - 后台运行程序"
	@echo "  make show     - 显示内存布局（需要进程运行中）"
	@echo "  make maps     - 直接查看 /proc/<pid>/maps"
	@echo "  make monitor  - 持续监控内存变化"
	@echo "  make stop     - 停止运行的进程"
	@echo ""
	@echo "分析工具:"
	@echo "  make status   - 查看进程状态"
	@echo "  make segments - 查看 ELF 段信息"
	@echo "  make symbols  - 查看符号表"
	@echo "  make compare  - 对比编译时和运行时地址"
	@echo ""
	@echo "其他:"
	@echo "  make clean    - 清理生成的文件"
	@echo "  make help     - 显示此帮助"
	@echo ""
	@echo "=========================================="
	@echo "推荐学习流程:"
	@echo "=========================================="
	@echo "1. make segments   - 先了解 ELF 文件的段结构"
	@echo "2. make symbols    - 查看符号地址"
	@echo "3. make run-bg     - 后台启动进程"
	@echo "4. make show       - 查看实际内存布局"
	@echo "5. make compare    - 对比编译时和运行时的差异"
	@echo "6. make monitor    - 实时监控（可选）"
	@echo ""
	@echo "或者直接:"
	@echo "  终端1: make run"
	@echo "  终端2: make show"
	@echo ""


