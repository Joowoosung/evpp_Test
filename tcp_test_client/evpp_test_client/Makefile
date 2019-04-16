CC = g++
INC = -I ../nt_lib/ -I ../lib/ 
CFLAGS = -c -Wall $(INC)
LDFLAGS = -lpthread
NT_LIB_OBJS = ../nt_lib/NT_MANAGER_CLIENT.o ../nt_lib/NT_MANAGER_SVR.o ../nt_lib/NT_MSG_Pool.o ../nt_lib/NT_TCPClient.o ../nt_lib/NT_TCPConn.o ../nt_lib/NT_TCPServer.o
TCP_SERVER_OBJECTS = bussiness_logic.o main.o
EVPP_LIB = ../lib/evpp_lib/libevpp.so

all: $(TCP_SERVER_OBJECTS) $(NT_LIB_OBJS) 
	(cd ../nt_lib ; make)
	$(CC) $(TCP_SERVER_OBJECTS) $(NT_LIB_OBJS) $(EVPP_LIB) $(LDFLAGS) -o tcp_client

clean:
	rm -f *.o
	rm -f tcp_client

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

