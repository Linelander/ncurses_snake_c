CC      = cc
CFLAGS  = -Wall -Wextra -std=c99
LDLIBS  = -lncurses
TARGET  = snake
SRC     = snake1.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean