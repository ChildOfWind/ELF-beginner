.PHONY: all clean run analyze

CC = gcc
CFLAGS = -Wall -g -no-pie

all: libmylib.so main

# 编译动态库
libmylib.so: mylib.c mylib.h
	$(CC) -shared -fPIC -o libmylib.so mylib.c

# 编译主程序
main: main.c libmylib.so
	$(CC) $(CFLAGS) -o main main.c -L. -lmylib -Wl,-rpath=.

# 运行程序
run: main
	@echo "运行程序..."
	./main

# 分析PLT和GOT
analyze: main libmylib.so
	@echo "================================"
	@echo "1. 查看 main 程序的 PLT 段"
	@echo "================================"
	objdump -d -j .plt main | head -50
	@echo ""
	@echo "================================"
	@echo "2. 查看 main 程序的 GOT 段"
	@echo "================================"
	objdump -s -j .got.plt main
	@echo ""
	@echo "================================"
	@echo "3. 查看动态符号表"
	@echo "================================"
	readelf -r main
	@echo ""
	@echo "================================"
	@echo "4. 查看需要的动态库"
	@echo "================================"
	ldd main

clean:
	rm -f main libmylib.so *.o

