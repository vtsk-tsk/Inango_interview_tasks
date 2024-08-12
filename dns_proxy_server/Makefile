CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGET = dns_proxy_server
SRC_DIR = src/

all: $(TARGET)

$(TARGET): dns_proxy_server.o
	$(CC) $(CFLAGS) -o $(TARGET) dns_proxy_server.o

dns_proxy_server.o: $(SRC_DIR)dns_proxy_server.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)dns_proxy_server.c

clean:
	rm -f *.o $(TARGET)

