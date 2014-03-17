#Makefile
#
# Instrucciones para compilar

OBJS1 = Cliente.o
OBJS2 = Server.c
CC = gcc
CFLAGS = -Wall -O2

all: $(OBJS1) Cliente
	

cliente: $(OBJS1)
	$(CC) $(CFLAGS) $^ -o cliente

servidor: $(OBJS2)
	$(CC) $(CFLAGS) $^ -o servidor

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) cliente
	rm -rf $(OBJS) servidor

push: 
	git add . 
	git commit -m 'Commit from make'
	git push

pull:
	git pull
