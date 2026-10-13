
CC = cc
CFLAGS = -Wall -Wextra -std=c11 -D_DEFAULT_SOURCE -Iinclude

TARGET = ls
SRC = src/main.c src/options.c src/list.c src/display.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
