/*
 * sslserver.c
 *
 *  Created on: 27/03/2014
 *      Author: eduardo
 */

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

#define FAIL    -1

int OpenListener(int port)
{   int sd;
    struct sockaddr_in addr;

    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        perror("can't bind port");
        abort();
    }
    if ( listen(sd, 10) != 0 )
    {
        perror("Can't configure listening port");
        abort();
    }
    return sd;
}

SSL_CTX* InitServerCTX(void)
{   SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = SSLv23_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
 /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}

void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}

void Servlet(SSL* ssl) /* Serve the connection -- threadable */
{   char buf[1024];
	char out[1024];
    char reply[1024];
    int firstFlag=1;
    char firstLine[1024];
    int sd, bytes;
    int readBytes;
    char *ptr;
    int file;
    const char* HTMLecho="<html><body><pre>%s</pre></body></html>\n\n";

    if ( SSL_accept(ssl) == FAIL ){     /* do SSL-protocol accept */
    	ERR_print_errors_fp(stderr);
    }

    else
    {
        ShowCerts(ssl);        /* get any certificates */

    	SSL_read(ssl, buf, sizeof(buf));

    	// Procesar el get

    	ptr = strtok(buf, "/");
    	ptr = strtok(NULL, " ");
    		fprintf(stdout,"%s\n",ptr);

    	//Responder con 200 ok si existe
    	//strcpy(out, "HTTP/1.1 200 ok\r\nContent-Type: text/html\r\n\r\n");
    	//sprintf(reply, HTMLecho, out);   /* construct reply */
    	//SSL_write(ssl, reply, strlen(reply));
    	//printf("Lo que mandó:  %s",reply);
    	//printf("1");
    	//strcpy(out, "Content-Type: text/html\r\n");
    	//sprintf(reply, HTMLecho, out);   /* construct reply */
    	//SSL_write(ssl, reply,strlen(reply));
    	//printf("lo que mandó:  %s",reply);
    	//strcpy(out, "Content-Lenght: 171\r\n");
    	//sprintf(reply, HTMLecho, out);   /* construct reply */
    	//SSL_write(ssl, reply,strlen(reply));
    	//printf("lo que mandó:  %s",reply);
    	//strcpy(out, "\r\n");
    	//sprintf(reply, HTMLecho, out);   /* construct reply */
    	//SSL_write(ssl, reply, strlen(reply));
    	//printf("lo que mandó:  %s",reply);

    	//Enviar el archivo
    	file = open(ptr, O_RDONLY);
    	//printf("2\n");
    	//RESPONDER CON 404 NOT FOUND SI NO EXISTE
    	if(file == -1){
    		strcpy(reply, "<html><head><title>404 NOT FOUND</title></head><body>NO EXISTE!!!</body></html>");
    		SSL_write(ssl, reply,strlen(reply));
    	}

    	else{
    		while((readBytes = read(file,buf, 255)) > 0){
    			SSL_write(ssl, buf, readBytes);
    		}
    	}
    }
    sd = SSL_get_fd(ssl);       /* get socket connection */
    SSL_free(ssl);         /* release SSL state */
    close(sd);          /* close connection */
}

int main(int count, char *strings[])
{   SSL_CTX *ctx;
    int server;
    char *portnum;

    if ( count != 2 )
    {
        printf("Usage: %s <portnum>\n", strings[0]);
        exit(0);
    }

    SSL_library_init();

    portnum = strings[1];

    ctx = InitServerCTX();        /* initialize SSL */

    LoadCertificates(ctx, "cert.pem", "key.pem"); /* load certs */

    server = OpenListener(atoi(portnum));    /* create server socket */

    while (1)
    {   struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;

        int client = accept(server, (struct sockaddr*)&addr, &len);  /* accept connection as usual */
        printf("Connection: %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        ssl = SSL_new(ctx);              /* get new SSL state with context */
        SSL_set_fd(ssl, client);      /* set connection socket to SSL state */
        Servlet(ssl);         /* service connection */
    }
    close(server);          /* close server socket */
    SSL_CTX_free(ctx);         /* release context */
}
