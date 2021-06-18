#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

void *inController(void *client_fd);
void *outController(void *client_fd);

int main(int argc, char *argv[])
{
	pthread_t tid[2];

	struct sockaddr_in address;
	int client_fd, opt = 1;
	struct hostent *host;

	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	host = gethostbyname("127.0.0.1");
	address.sin_addr = *((struct in_addr *)host->h_addr);

	if (connect(client_fd, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) == -1)
		exit(EXIT_FAILURE);

	pthread_create(&(tid[0]), NULL, &inController, (void *)&client_fd);
	pthread_create(&(tid[1]), NULL, &outController, (void *)&client_fd);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	close(client_fd);
	return 0;
}

void *inController(void *client_fd)
{
	char query[1000] = {0};
	int fd = *(int *)client_fd;

	printf("Welcome to database A09\n");

	while (1)
	{
		printf("A09> ");

		fgets(query, 1000, stdin);
		char *temp = strtok(query, "\n");

		if (temp != NULL)
		{
			strcpy(query, temp);
		}

		if (strcmp(query, "exit") == 0)
		{
			exit(EXIT_SUCCESS);
		}

		send(fd, query, sizeof(char) * 1000, 0);
	}
}

void *outController(void *client_fd)
{
	char query[1000] = {0};
	int fd = *(int *)client_fd;

	while (1)
	{
		memset(query, 0, sizeof(char) * 1000);

		if (recv(fd, query, 1000, 0) == 0)
			exit(EXIT_SUCCESS);

		printf("%s", query);
		fflush(stdout);
	}
}
