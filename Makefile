#Makefile
#
# Instrucciones para compilar

OBJS = tcp.o server.o main.o
CC = gcc
CFLAGS = -Wall -O2

all: 

push: 
	git add . 
	git commit -m 'Commit from make'
	git push https://github.com/rt688375/Sockets.git

pull:
	git pull
