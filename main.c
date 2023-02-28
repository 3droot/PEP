/*
 * main.c
 *
 *  Created on: Feb 27, 2023
 *      Author: trevor
 */


#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "ini.h"
#define MAX 80
#define SA struct sockaddr


typedef struct
{
	int port;
	char* p1;
	char* p2;
} configuration;

static int handler(void* user, const char* section, const char* name,
		const char* value)
{
	configuration* pconfig = (configuration*)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	if (MATCH("address", "port")) {
		pconfig->port = atoi(value);
	} else if (MATCH("prefix", "p1")) {
		pconfig->p1 = strdup(value);
	} else if (MATCH("prefix", "p2")) {
		pconfig->p2 = strdup(value);
	} else {
		return 0;  /* unknown section/name, error */
	}
	return 1;
}

void func(int connfd)
{
	char buff[MAX];
	char dest[3];
	char data[MAX];
	int n;
	bzero(buff, MAX);
	bzero(data, MAX);
	FILE* file;

	while(1) {


		read(connfd, buff, sizeof(buff));
		printf("Here\n");
		strncpy(dest, buff, 2);
		printf("From client: %s\n", dest);
		strncpy(data, buff+2, 80);
		printf("%s\n", data);
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		bzero(buff, MAX);
		if(strcpy(dest, "p1"))
		{
			file = fopen("data.txt", "a+");
			sprintf(buff, "%d:%d -> %s",tm.tm_hour, tm.tm_min, data);
			fwrite(buff, strlen(buff),1, file);
			fclose(file);



		}
		else if(strcpy(dest, "p2"))
		{
			       file = fopen("data.txt", "a+");
					sprintf(buff, "%d:%d -> \'%s\' ",tm.tm_hour, tm.tm_min, data);
					fwrite(buff, strlen(buff),1, file);
					fclose(file);
		}

		else
			continue;

		bzero(buff, MAX);
		bzero(dest, 3);
	}

}

int main()
{
	int sockfd, connfd, len;
	int res;
	struct sockaddr_in servaddr, cli;
	configuration config;

	res = ini_parse("fconfig.ini", handler, &config);
	if(res < 0) {
		printf("Can't load 'test.ini'\n");
		return EXIT_FAILURE;
	}

	printf("%d\n", config.port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(config.port);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	func(connfd);

	close(sockfd);
}
