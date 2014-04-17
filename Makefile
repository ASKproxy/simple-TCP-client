CC = gcc
CFLAGS = -g
EXEC = client
OBJS = client.o
SRCS = client.c

$(EXEC) : $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
$(OBJS) : $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)
client : $(SRCS)
	$(CC) $(CFLAGS) -g -c $(SRCS)
	$(CC) $(CFLAGS) -g -o $(EXEC) $(OBJS)
clean :
	rm -f *.o