CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 

TARGET = s21_grep

SRCS = s21_grep.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) -o $@ $^
		rm -rf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	cd myTest_grep && chmod +x ./test.sh && bash ./test.sh 1

test_memory: $(TARGET)
	cd myTest_grep && chmod +x ./test.sh && bash ./test.sh 2
	
clean:
	rm -f $(OBJS) $(TARGET) && cd myTest_grep && rm -f test_memory.log && rm -f test.log

.PHONY: all clean
