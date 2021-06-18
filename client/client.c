#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#define PORT 8080

const int SIZE_BUFFER = sizeof(char) * 1000;
bool wait = false;

void *handleInput(void *client_fd);
void *handleOutput(void *client_fd);

int main(int argc, char *argv[])
{
	pthread_t tid[2];

	struct sockaddr_in address;
	int client_fd, opt = 1;
	struct hostent *local_host;

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
	local_host = gethostbyname("127.0.0.1");
	address.sin_addr = *((struct in_addr *)local_host->h_addr);

	if (connect(client_fd, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) == -1)
	{
		exit(EXIT_FAILURE);
	}

	pthread_create(&(tid[0]), NULL, &handleOutput, (void *)&client_fd);
	pthread_create(&(tid[1]), NULL, &handleInput, (void *)&client_fd);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	close(client_fd);
	return 0;
}

void *handleInput(void *client_fd)
{
	int fd = *(int *)client_fd;
	char query[1000] = {0};
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

		send(fd, query, SIZE_BUFFER, 0);
		wait = true;
	}
}

void *handleOutput(void *client_fd)
{
	int fd = *(int *)client_fd;
	char query[1000] = {0};

	while (1)
	{
		memset(query, 0, SIZE_BUFFER);
		if (recv(fd, query, 1000, 0) == 0)
		{
			exit(EXIT_SUCCESS);
		}
		printf("%s", query);
		fflush(stdout);
		wait = false;
	}
}
