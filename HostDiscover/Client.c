/*
 * Client.c
 *
 *  Created on: 18/03/2014
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

#include "Sockets.h"

int main(int args, char *argv[]) {

	u_int puerto;
	int udpSocket;

	struct sockaddr_in sender;
	socklen_t senderlen = sizeof(sender);
	u_int senderPort;

	char buffer [255];
	char senderIp [20];

	int status;

	if(args < 2) {
		fprintf(stderr, "Error: Missing Arguments\n");
		fprintf(stderr, "\tUse: %s [PORT]\n", argv[0]);
		return	1;
	}

	puerto = atoi(argv[1]);
	if(puerto < 1 || puerto > 65535) {
		fprintf(stderr, "Port %i out of range (1-65535)\n",puerto);
		return	1;
	}

	udpSocket = UDPnewSockServ(puerto);

	while(1){
		bzero(buffer, 255);
		status = recvfrom(udpSocket, buffer, 255, 0, (struct sockaddr *)&sender, &senderlen);

		inet_ntop(AF_INET, &(sender.sin_addr), senderIp, INET_ADDRSTRLEN);
		senderPort = ntohs(sender.sin_port);

		fprintf(stderr, "Recibimos de [%s:%i] el mensaje %s\n", senderIp, senderPort, buffer);

		status = sendto(udpSocket, "LALO", 40, 0, (struct sockaddr *)&sender, senderlen);

	}

	close(udpSocket);

}
