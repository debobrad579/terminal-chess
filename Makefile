CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
OBJ = build/main.o build/board.o build/move_piece.o build/can_move.o build/legal_moves.o
TARGET = main

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

.PHONY: clean run

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build $(TARGET)
