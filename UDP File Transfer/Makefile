#Variable para guardar los objetos a compilar
OBJS = UDP_Server.c
OBJS2 = UDP_Client.c

#Variable para el compulador
CC = gcc

#Variable para guardar las opciones del compilador
CCFLAGS = -Wall -o
#-Wall indica al compilador que muestre todos los warnings existentes

all: $(OBJS) Server Client

Server: $(OBJS)
		$(CC) $(OBJS) $(CCFLAGS) UDPServer
Client: $(OBJS2)
		$(CC) $(OBJS2) $(CCFLAGS) UDPClient
