CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -g

chatclient: chatclient.c util.h
	$(CC) $(CFLAGS) chatclient.c util.h -o chatclient
chatserver: chatserver.c util.h
	$(CC) $(CFLAGS) chatserver.c util.h -o chatserver
clean:
	rm -f $(TARGET) $(TARGET).exe
