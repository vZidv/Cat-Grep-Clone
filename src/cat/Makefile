CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11

TARGET = s21_cat

SRCS = s21_cat.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) -o $@ $^
		rm -f $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	cd myTest_cat && chmod +x ./test.sh && bash ./test.sh 1

test_memory: $(TARGET)
	cd myTest_cat && chmod +x ./test.sh && bash ./test.sh 2

clean:
	rm -f $(OBJS) $(TARGET) && cd myTest_cat && rm -f test_memory.log && rm -f test.log