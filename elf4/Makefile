# Makefile for studying ELF relocation
# 用于研究 ELF 重定位

CC = gcc
CFLAGS = -Wall -g -fno-pie
LDFLAGS = -no-pie

# 目标文件
OBJS = a.o b.o
TARGET = program

# 默认目标：编译并链接
all: $(TARGET)
	@echo "=== 编译完成 ==="
	@echo "运行 'make show' 查看重定位信息"

# 链接生成可执行文件
$(TARGET): $(OBJS)
	@echo "=== 链接目标文件 ==="
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# 编译 a.c -> a.o
a.o: a.c b.h
	@echo "=== 编译 a.c ==="
	$(CC) $(CFLAGS) -c a.c -o a.o

# 编译 b.c -> b.o
b.o: b.c b.h
	@echo "=== 编译 b.c ==="
	$(CC) $(CFLAGS) -c b.c -o b.o

# 运行程序
run: $(TARGET)
	@echo "=== 运行程序 ==="
	./$(TARGET)

# 显示重定位信息（核心部分）
show: $(OBJS) $(TARGET)
	@echo "========================================"
	@echo "=== a.o 的重定位表 ==="
	@echo "========================================"
	readelf -r a.o
	@echo ""
	@echo "========================================"
	@echo "=== b.o 的重定位表 ==="
	@echo "========================================"
	readelf -r b.o
	@echo ""
	@echo "========================================"
	@echo "=== a.o 的符号表 ==="
	@echo "========================================"
	readelf -s a.o
	@echo ""
	@echo "========================================"
	@echo "=== b.o 的符号表 ==="
	@echo "========================================"
	readelf -s b.o
	@echo ""
	@echo "========================================"
	@echo "=== 最终可执行文件的符号表 ==="
	@echo "========================================"
	readelf -s $(TARGET)

# 显示详细的反汇编信息
disasm: $(OBJS) $(TARGET)
	@echo "========================================"
	@echo "=== a.o 反汇编（查看重定位前） ==="
	@echo "========================================"
	objdump -d -r a.o
	@echo ""
	@echo "========================================"
	@echo "=== b.o 反汇编（查看重定位前） ==="
	@echo "========================================"
	objdump -d -r b.o
	@echo ""
	@echo "========================================"
	@echo "=== program 反汇编（查看重定位后） ==="
	@echo "========================================"
	objdump -d $(TARGET)

# 只显示重定位条目
reloc: $(OBJS)
	@echo "=== a.o 中需要重定位的符号 ==="
	@objdump -r a.o | grep -A 20 "RELOCATION"
	@echo ""
	@echo "=== b.o 中需要重定位的符号 ==="
	@objdump -r b.o | grep -A 20 "RELOCATION"

# 显示 ELF 文件头信息
header: $(TARGET)
	@echo "=== ELF 文件头信息 ==="
	readelf -h $(TARGET)
	@echo ""
	@echo "=== 节（Section）头信息 ==="
	readelf -S $(TARGET)

# 清理
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "=== 清理完成 ==="

# 帮助信息
help:
	@echo "Makefile 使用说明："
	@echo "  make          - 编译并链接程序"
	@echo "  make run      - 编译并运行程序"
	@echo "  make show     - 显示重定位和符号表信息（推荐）"
	@echo "  make disasm   - 显示详细的反汇编信息"
	@echo "  make reloc    - 只显示重定位条目"
	@echo "  make header   - 显示 ELF 文件头信息"
	@echo "  make clean    - 清理生成的文件"
	@echo "  make help     - 显示此帮助信息"
	@echo ""
	@echo "学习重定位的步骤："
	@echo "  1. make          - 编译生成 .o 文件和可执行文件"
	@echo "  2. make show     - 查看重定位表，观察哪些符号需要重定位"
	@echo "  3. make disasm   - 查看汇编代码，对比重定位前后的地址变化"

.PHONY: all run show disasm reloc header clean help

