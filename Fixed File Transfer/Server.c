/*
 * Server.c
 *
 *  Created on: 12/03/2014
 *      Author: eduardo
 */

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

int main(int args, char* argv[]){

	int tcpSocket, Cliente;
	struct sockaddr_in Server_addr;
	struct sockaddr_in Client_addr;
	int status;
	u_int puerto;
	socklen_t clienteLen;
	char *cadena;
	char archivo [] = "archivo.txt";

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

	while(1){

		bzero(&Client_addr, sizeof(Client_addr));
		Cliente = accept(tcpSocket, (struct sockaddr *)&Client_addr, &clienteLen);

		cadena = (char *) calloc(1,10);
		cadena = "READY\r\n";
		write(Cliente, cadena, strlen(cadena));
		fprintf(stderr,"Enviado: %s",cadena);

		cadena = (char *) calloc(1,10);
		read(Cliente, cadena, 4);
		fprintf(stderr,"Recivido: %s",cadena);

		if((strcmp(cadena, "OK\r\n"))==0){

			if((file = open(archivo, O_RDONLY)) ==-1){

				localerror = errno;
				fprintf(stderr, "No se pudo abrir el archivo para mandar (%s)\n", strerror(localerror));
				return 0;
			}

			readBuffer = (char *) calloc(1, 1024);

			while((readBytes = read(file,readBuffer,1024)) > 0){
				totalWrotenBytes = 0;
				while(totalWrotenBytes < readBytes){
					wrotenBytes = write(Cliente,readBuffer+totalWrotenBytes, readBytes-totalWrotenBytes);
					totalWrotenBytes += wrotenBytes;
				}
				totalReadBytes += readBytes;
			}

			fprintf(stderr,"Se mandaron %d Bytes\n",totalReadBytes);
			free(readBuffer);
			close(file);
			close(Cliente);

		}

		else{
			close(Cliente);
		}

	}

	return 0;
}
