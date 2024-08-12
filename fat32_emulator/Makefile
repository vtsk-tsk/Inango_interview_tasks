CC = gcc
CFLAGS = -Wall -Wextra

TARGET = f32disk
SRC_DIR = src/

all: $(TARGET)

$(TARGET): main.o fat32.o cli.o
	$(CC) -o f32disk main.o fat32.o cli.o $(CFLAGS)

main.o: $(SRC_DIR)main.c $(SRC_DIR)fat32.h $(SRC_DIR)cli.h
	$(CC) -c $(SRC_DIR)main.c $(CFLAGS)

fat32.o: $(SRC_DIR)fat32.c $(SRC_DIR)fat32.h
	$(CC) -c $(SRC_DIR)fat32.c $(CFLAGS)

cli.o: $(SRC_DIR)cli.c $(SRC_DIR)cli.h $(SRC_DIR)fat32.h
	$(CC) -c $(SRC_DIR)cli.c $(CFLAGS)

clean:
	rm -f *.o $(TARGET)
