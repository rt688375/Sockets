/*
 * UDP_Server.c
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

int main(int agrs, int argv[]){

	int udpSocket;
	u_int puerto;
	int status;
	struct sockaddr_in Cliente;
	socklen_t Clientelen = sizeof(Cliente);

	int file;
	struct stat fileStats;
	char *fileSize;
	char *Cadena;
	char *buffer;
	int readBytes, totalReadBytes;
	int writeBytes, totalWriteBytes;
	int i = 1;
	char part [10];

	puerto = atoi(argv[1]);

	udpSocket = UDPnewSockServ(puerto);

	status = recvfrom(udpSocket, Cadena, sizeof(Cadena), 0, (struct sockaddr *)&Cliente, Clientelen);

	file = open(Cadena, O_RDONLY);
	if(file == -1){
		strcpy(Cadena, "The file doesn't exist");
		closeUDPSocket4(udpSocket);
		return 1;
	}
	else
		strcpy(Cadena, "OK, file found");

	status = sendto(udpSocket, Cadena, 100, 0, (struct sockaddr *)&Cliente, Clientelen);

	fstat(file, &fileStats);

	sprintf(fileSize, "%jd", (intmax_t)fileStats.st_size);
	printf("File Size: %s\n", fileSize);
	status = sendto(udpSocket, fileSize, sizeof(fileSize), 0, (struct sockaddr *)&Cliente, Clientelen);

	buffer = (char *)calloc(1, 1024);

	totalReadBytes = 0;
	while((readBytes = read(file, buffer, 1024)) > 0) {
		totalWriteBytes = 0;
		sprintf(part, "%i", i);
		printf("Sending package number: %s\n", part);
		while(totalWriteBytes < readBytes) {
			writeBytes = sendto(udpSocket, buffer+totalWriteBytes, readBytes-totalWriteBytes,0, (struct sockaddr *)&Cliente, Clientelen);
			totalWriteBytes += writeBytes;
			status = sendto(udpSocket, part, 6, 0, (struct sockaddr *)&Cliente, Clientelen);
		}
		status = recvfrom(udpSocket, Cadena, sizeof(Cadena), 0, (struct sockaddr *)&Cliente, &Clientelen);
		if(strcmp(Cadena, "ERROR\r\n")==0)	{
			printf("Transmission Error\n");
			closeUDPSocket4(udpSocket);
			return 1;
		}
		totalReadBytes += readBytes;
		i++;
	}

	close(udpSocket);


	return 0;
}
