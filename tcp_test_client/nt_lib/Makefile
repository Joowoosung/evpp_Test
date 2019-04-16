CC = g++
INC = -I ../lib/
CFLAGS = -c -Wall $(INC)
LDFLAGS = 
NT_LIB_OBJS = NT_MANAGER_CLIENT.o NT_MANAGER_SVR.o NT_MSG_Pool.o NT_TCPClient.o NT_TCPConn.o NT_TCPServer.o
EVPP_LIB = ../evpp/libevpp.so

all: $(NT_LIB_OBJS) $(TCP_SERVER_OBJECTS)

clean:
		rm -f *.o

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

