.PHONY: all clean run debug info

CC = gcc
CFLAGS = -g -O0 -Wall -Wextra
TARGET = auxv_demo

all: $(TARGET)

$(TARGET): auxv_demo.c
	$(CC) $(CFLAGS) -o $(TARGET) auxv_demo.c

# 运行程序
run: $(TARGET)
	@echo "=== 运行 auxv_demo ==="
	./$(TARGET)

# 使用 GDB 调试
debug: $(TARGET)
	@echo "=== 使用 GDB 调试 ==="
	gdb -ex "break main" -ex "run" ./$(TARGET)

# 查看 auxv 信息（通过 /proc）
info: $(TARGET)
	@echo "=== 查看进程 auxv 信息 ==="
	@./$(TARGET) & PID=$$!; sleep 0.1; \
	echo "进程 PID: $$PID"; \
	echo "\n--- /proc/$$PID/auxv (十六进制) ---"; \
	od -t x8 /proc/$$PID/auxv; \
	wait $$PID

# 查看 ELF 文件信息
elf: $(TARGET)
	@echo "=== ELF 文件头 ==="
	readelf -h $(TARGET)
	@echo "\n=== 程序头表 ==="
	readelf -l $(TARGET)
	@echo "\n=== 节头表 ==="
	readelf -S $(TARGET)

# 反汇编主函数
disasm: $(TARGET)
	@echo "=== 反汇编 main 函数 ==="
	objdump -d -M intel $(TARGET) | grep -A 50 "<main>:"

clean:
	rm -f $(TARGET) *.o

# 帮助信息
help:
	@echo "可用的 make 目标:"
	@echo "  make          - 编译程序"
	@echo "  make run      - 运行程序"
	@echo "  make debug    - 使用 GDB 调试"
	@echo "  make info     - 查看进程的 auxv 信息"
	@echo "  make elf      - 查看 ELF 文件信息"
	@echo "  make disasm   - 反汇编 main 函数"
	@echo "  make clean    - 清理生成的文件"

