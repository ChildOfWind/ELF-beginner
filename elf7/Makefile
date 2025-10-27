.PHONY: all clean run compare analyze

CC = gcc
CFLAGS = -Wall -g -no-pie

all: libtest.so main_lazy main_now

# 编译动态库
libtest.so: libtest.c libtest.h
	$(CC) -shared -fPIC -o libtest.so libtest.c


# 真正的延迟绑定版本（禁用 RELRO）
main_lazy: main.c libtest.so
	$(CC) $(CFLAGS) -o main_lazy main.c -L. -ltest -Wl,-rpath=. -Wl,-z,norelro
	@echo "已创建真正延迟绑定版本: main_lazy (禁用RELRO)"

# 立即绑定版本（严格模式）
main_now: main.c libtest.so
	$(CC) $(CFLAGS) -o main_now main.c -L. -ltest -Wl,-rpath=. -Wl,-z,now
	@echo "已创建立即绑定版本: main_now"

# 运行所有版本
run: main_lazy main_now
	@echo "========================================"
	@echo "1. 运行默认版本 (main_lazy)"
	@echo "========================================"
	./main_lazy
	@echo ""
	@echo "========================================"
	@echo "2. 运行立即绑定版本 (main_now)"
	@echo "========================================"
	./main_now

# 对比两个版本
compare: main_lazy main_now
	@echo "========================================"
	@echo "对比延迟绑定 vs 立即绑定"
	@echo "========================================"
	@./compare.sh

# 详细分析
analyze: main_lazy main_now
	@echo "========================================"
	@echo "PLT/GOT 详细分析"
	@echo "========================================"
	@./analyze_binding.sh

clean:
	rm -f main_lazy main_now libtest.so *.o

