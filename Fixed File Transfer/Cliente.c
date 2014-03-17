/*
 * Cliente.c
 *
 *  Created on: 12/03/2014
 *      Author: eduardo
 */

#include <stdio.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>

int main(int args, char* argv[]){

	int tcpSocket;
	struct sockaddr_in Client_addr;
	u_int puerto;
	char *cadena;

	int localerror;

	int file;
	char *readBuffer = NULL;
	int totalReadBytes = 0;
	int totalWrotenBytes = 0;
	int readBytes = 0;
	int wrotenBytes = 0;

	tcpSocket = socket(PF_INET, SOCK_STREAM, 0);

	if(tcpSocket == -1) {
		localerror = errno;
		fprintf(stderr, "Error: %s\n",strerror(localerror));
		return 1;
	}

	puerto = atoi(argv[1]);

	bzero(&Client_addr, sizeof(Client_addr));
	Client_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[2], &(Client_addr.sin_addr.s_addr));
	Client_addr.sin_port = htons(puerto);

	connect(tcpSocket, (struct sockaddr *)&Client_addr, sizeof(Client_addr));

	cadena = (char *) calloc(1,10);
	read(tcpSocket, cadena, 10);

	fprintf(stderr,"Recivido: %s",cadena);

	if((strcmp(cadena,"READY\r\n"))==0){

		cadena = "OK\r\n";
		write(tcpSocket, cadena, strlen(cadena));

		fprintf(stderr,"Enviado: %s",cadena);

		if((file = open("recivido.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) ==-1){

			localerror = errno;
			fprintf(stderr, "No se pudo abrir el archivo para mandar (%s)\n", strerror(localerror));
			return 0;
		}

		readBuffer = (char *) calloc(1,1024);

		while((readBytes = read(tcpSocket,readBuffer,1024)) > 0) {
			totalWrotenBytes = 0;
			while(totalWrotenBytes < readBytes) {
				wrotenBytes = write(file,readBuffer+totalWrotenBytes,readBytes-totalWrotenBytes);
				totalWrotenBytes += wrotenBytes;
			}
			totalReadBytes += readBytes;
		}

		fprintf(stderr,"Se recivieron %d Bytes\n",totalReadBytes);

		close(tcpSocket);
		close(file);
		return 0;

	}


	return 0;
}
