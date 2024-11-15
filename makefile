# 定义编译器和编译选项
CC = gcc
CFLAGS = -g -Wall

# 定义源文件
SRCS = test.c mail.c utils.c

# 定义目标文件
OBJS = $(SRCS:.c=.o)

# 定义最终目标
TARGET = test

# 默认目标
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 生成 .o 文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理目标文件
clean:
	rm -f $(OBJS) $(TARGET)

# 声明依赖关系
test.o: mail.h utils.h
mail.o: mail.h utils.h
