/*
 * Sockets.h
 *
 *  Created on: 16/03/2014
 *      Author: eduardo
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdio.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>

int TCPnewSockServ(u_int puerto){

	int tcpSocket;
	struct sockaddr_in Server_addr;
	int localerror;
	int status;

	tcpSocket = socket(PF_INET, SOCK_STREAM, 0);

	if(tcpSocket == -1) {
		localerror = errno;
		fprintf(stderr, "Error: %s\n",strerror(localerror));
		return 1;
	}

	bzero(&Server_addr, sizeof(Server_addr));
	Server_addr.sin_family = AF_INET;
	Server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	Server_addr.sin_port = htons(puerto);

	status = bind(tcpSocket, (struct sockaddr *)&Server_addr, sizeof(Server_addr));

	if(status != 0) {
		localerror = errno;
		fprintf(stderr,"Can't Bind Port: %s\n",strerror(localerror));
		close(tcpSocket);
		return 1;
	}

	status = listen(tcpSocket,5);
	if(status == -1) {
		localerror = errno;
		fprintf(stderr,"Can't listen on socket(%s)\n",strerror(localerror));
		close(tcpSocket);
		return 1;
	}

	return tcpSocket;
}

int TCPnewSockCli(u_int puerto, char* ip){

	int tcpSocket;
	int localerror;
	struct sockaddr_in Client_addr;

	tcpSocket = socket(PF_INET, SOCK_STREAM, 0);

	if(tcpSocket == -1) {
		localerror = errno;
		fprintf(stderr, "Error: %s\n",strerror(localerror));
		return 1;
	}

	bzero(&Client_addr, sizeof(Client_addr));
	Client_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(Client_addr.sin_addr.s_addr));
	Client_addr.sin_port = htons(puerto);

	connect(tcpSocket, (struct sockaddr *)&Client_addr, sizeof(Client_addr));

	return tcpSocket;

}

int UDPnewSockServ(u_int puerto){

	int udpSocket;
	struct sockaddr_in Server_addr;
	int localerror;
	int status;


	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if(udpSocket == -1) {
		localerror = errno;
		fprintf(stderr, "Error: %s\n",strerror(localerror));
		return 1;
	}

	Server_addr.sin_family = AF_INET;
	Server_addr.sin_port = htons(puerto);
	Server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	status = bind(udpSocket, (struct sockaddr*)&Server_addr, sizeof(Server_addr));

	if(status != 0) {
		localerror = errno;
		fprintf(stderr,"Can't Bind Port: %s\n",strerror(localerror));
		close(udpSocket);
		return 1;
	}

	return udpSocket;
}

int UDPnewSockCli(){

	int udpSocket;
	struct sockaddr_in Client_addr;
	int localerror;
	int status;

	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if(udpSocket == -1) {
		localerror = errno;
		fprintf(stderr, "Error: %s\n",strerror(localerror));
		return 1;
	}

	Client_addr.sin_family = AF_INET;
	Client_addr.sin_port = 0;
	Client_addr.sin_addr.s_addr = INADDR_ANY;

	status = bind(udpSocket, (struct sockaddr*)&Client_addr, sizeof(Client_addr));

	if(status != 0) {
		localerror = errno;
		fprintf(stderr,"Can't Bind Port: %s\n",strerror(localerror));
		close(udpSocket);
		return 1;
	}

	printf("Client UDP Socket Created\n");

	return udpSocket;
}

int acceptClient(tcpSocket){

	int Cliente;
	struct sockaddr_in Client_addr;
	socklen_t clienteLen;

	bzero(&Client_addr, sizeof(Client_addr));
	Cliente = accept(tcpSocket, (struct sockaddr *)&Client_addr, &clienteLen);

	return Cliente;
}

int openArchCli(char *nombre){

	int file;
	int localerror;

	if((file = open(nombre, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) ==-1){

		localerror = errno;
		fprintf(stderr, "No se pudo abrir el archivo para recivir (%s)\n", strerror(localerror));
		return -1;
	}

	return file;

}

int openArchServ(char *nombre){

	int file;
	int localerror;

	if((file = open(nombre, O_RDONLY)) ==-1){

		localerror = errno;
		fprintf(stderr, "No se pudo abrir el archivo para mandar (%s)\n", strerror(localerror));
		return 0;
	}

	return file;
}

void sendFile(int file, int cliente){

	char *readBuffer = NULL;
	int totalReadBytes = 0;
	int totalWrotenBytes = 0;
	int readBytes = 0;
	int wrotenBytes = 0;

	readBuffer = (char *) calloc(1, 1024);

	while((readBytes = read(file,readBuffer,1024)) > 0){
		totalWrotenBytes = 0;
		while(totalWrotenBytes < readBytes){
			wrotenBytes = write(cliente,readBuffer+totalWrotenBytes, readBytes-totalWrotenBytes);
			totalWrotenBytes += wrotenBytes;
		}
		totalReadBytes += readBytes;
	}

	fprintf(stderr,"Se mandaron %d Bytes\n",totalReadBytes);

}

void recvFile(int file, int cliente){

	char *readBuffer = NULL;
	int totalReadBytes = 0;
	int totalWrotenBytes = 0;
	int readBytes = 0;
	int wrotenBytes = 0;

	readBuffer = (char *) calloc(1,1024);

	while((readBytes = read(cliente,readBuffer,1024)) > 0) {
		totalWrotenBytes = 0;
		while(totalWrotenBytes < readBytes) {
			wrotenBytes = write(file,readBuffer+totalWrotenBytes,readBytes-totalWrotenBytes);
			totalWrotenBytes += wrotenBytes;
		}
		totalReadBytes += readBytes;
	}

	fprintf(stderr,"Se recivieron %d Bytes\n",totalReadBytes);
}



#endif /* SOCKETS_H_ */
