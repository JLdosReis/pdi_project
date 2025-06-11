CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm
TARGET = decode
SRC = decode.c imagelib.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) *-result.pgm