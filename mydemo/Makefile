# 编译器设置
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -pthread

# 目标文件
TARGET := server

# 源文件
SRCS := server.cpp

# 默认目标
all: $(TARGET)

# 编译主程序
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 清理生成的文件
clean:
	rm -f $(TARGET)

# 重新编译
rebuild: clean all

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 安装到系统路径（需要sudo）
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# 显示帮助信息
help:
	@echo "可用命令:"
	@echo "  make          编译程序"
	@echo "  make clean    清理生成的文件"
	@echo "  make rebuild  重新编译"
	@echo "  make run      编译并运行"
	@echo "  make install  安装到系统路径"
	@echo "  make help     显示此帮助信息"

.PHONY: all clean rebuild run install help

