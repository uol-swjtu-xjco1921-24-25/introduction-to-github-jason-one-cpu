CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
EXEC = maze
OBJS = main.o maze.o file_io.o game_logic.o utils.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) *.o

.PHONY: all clean