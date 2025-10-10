#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE_RX 200
#define BUFSIZE_TX 256

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	printf("Starting server...\n");

	int sockfd, newsockfd, portno;

	socklen_t clilen;
	uint8_t bufferRx[BUFSIZE_RX];
	uint8_t bufferTx[BUFSIZE_TX];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	printf("Binding...\n");
	bzero((char *)&serv_addr, sizeof(serv_addr));
	portno = 3000;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	printf("Listen...\n");
	listen(sockfd, 5);

	clilen = sizeof(cli_addr);

	for (;;)
	{
		printf("Accept...\n");
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		if (newsockfd < 0)
			error("ERROR on accept");
		else
			printf("Accepted\n");

		bzero(bufferRx, sizeof(bufferRx));
		n = read(newsockfd, bufferRx, sizeof(bufferRx));

		if (n < 0)
			error("ERROR reading from socket");
		printf("Message: %s\n", (char *)bufferRx);

		snprintf((char *)bufferTx, sizeof(bufferTx), "Got message: %s", (char *)bufferRx);

		n = write(newsockfd, bufferTx, strlen((char *)bufferTx));
		if (n < 0)
			error("ERROR writing to socket");

		close(newsockfd);
	}
	close(sockfd);
	return 0;
}
