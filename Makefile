CC = gcc
CFLAGS = -lpthread
OBJS= Dandi.o DownFromFTP.o DownFromHTTP.o DandiBase.o GenFunc.o
all: linuxdown

linuxdown: $(OBJS)
	$(CC) $(CFLAGS)  -o linuxdown $(OBJS)
install:
	cp ./linuxdown /usr/bin
clean:
	rm -f $(EXEC) *.o
	rm -f /usr/bin/linuxdown
