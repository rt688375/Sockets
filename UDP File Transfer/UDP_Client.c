/*
 * UDP_Client.c
 *
 *  Created on: 10/03/2014
 *      Author: eduardo
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "Sockets.h"

int main(int args, char *argv[]){

	u_int puerto;
	int udpCliente;
	int status;
	char *Cadena;

	struct sockaddr_in Server_addr;
	int readBytes, totalReadBytes = 0;
	int writeBytes, totalWriteBytes;
	char *readBuffer = NULL;
	int file;
	char fileSize [50];

	char parte [10];
	int contadorParte = 1;

	if(args < 4) {
		fprintf(stderr, "Error: Missing Arguments\n");
		fprintf(stderr, "\tUse: %s [Port] [IP ADDRESS] [FILE]\n", argv[0]);
		return	1;
	}

	puerto = atoi(argv[1]);

	udpCliente = UDPnewSockCli();

	Server_addr.sin_family = AF_INET;
	Server_addr.sin_port = htons(puerto);
	inet_pton(AF_INET, argv[3], &(Server_addr.sin_addr.s_addr));

	socklen_t Serverlen = sizeof(Server_addr);

	status = sendto(udpCliente, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&Server_addr, sizeof(Server_addr));

	Cadena = (char *) calloc(1,10);

	status = recvfrom(udpCliente, Cadena, strlen(Cadena), 0, (struct sockaddr *)&Server_addr, &Serverlen);

	status = recvfrom(udpCliente, fileSize, sizeof(fileSize), 0, (struct sockaddr *)&Server_addr, &Serverlen);

	if((file = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1) {
		printf("Can't create file!\n");
		return 1;
	}

	readBuffer = (char *) calloc(1, 1024);

	while((readBytes = recvfrom(udpCliente, readBuffer, 1024, 0, (struct sockaddr *)&Server_addr, &Serverlen)) > 0) {
		totalWriteBytes = 0;
		printf("reading...\n");
		status = recvfrom(udpCliente, parte, 10, 0, (struct sockaddr *)&Server_addr, &Serverlen);
		printf("Receiving package number: %i of %i\n", atoi(parte), contadorParte);
		while(totalWriteBytes < readBytes) {
			writeBytes = write(file, readBuffer+totalWriteBytes, readBytes-totalWriteBytes);
			totalWriteBytes += readBytes;
		}
		if(atoi(parte) == contadorParte){
			status = sendto(udpCliente, "OK\r\n", 5, 0, (struct sockaddr *)&Server_addr, sizeof(Serverlen));
		}
		else
			status = sendto(udpCliente, "ERROR\r\n", 5, 0, (struct sockaddr *)&Server_addr, sizeof(Serverlen));
		totalReadBytes += readBytes;
		contadorParte++;
		if(totalReadBytes == atoi(fileSize))
			break;
	}

	return 0;
}
