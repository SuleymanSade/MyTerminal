CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -std=c17

# All .c files in this directory
SRCS = $(wildcard src/*.c)

# Corresponding .o files
OBJS = $(SRCS:.c=.o)

TARGET = program

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Generic rule: any .o from .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./program

debug:
	# This doesn't work yet, I am trying to fix it
	valgrind --leak-check=full ./program

clean:
	rm -f $(OBJS) $(TARGET)