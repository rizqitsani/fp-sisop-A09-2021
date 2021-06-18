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
#define DATA_BUFFER 300

const int SIZE_BUFFER = sizeof(char) * DATA_BUFFER;
char username[DATA_BUFFER] = {0};
bool wait = false;

// SETUP
int create_tcp_client_socket();
void *handleInput(void *client_fd);
void *handleOutput(void *client_fd);
void getServerOutput(int fd, char *input);

// Controller
bool login(int, int, char *[]);

int main(int argc, char *argv[])
{
	pthread_t tid[2];
	int client_fd = create_tcp_client_socket();

	if (!login(client_fd, argc, argv))
	{
		return -1;
	}

	pthread_create(&(tid[0]), NULL, &handleOutput, (void *)&client_fd);
	pthread_create(&(tid[1]), NULL, &handleInput, (void *)&client_fd);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	close(client_fd);
	return 0;
}

/**    CONTROLLER    **/
bool login(int fd, int argc, char *argv[])
{
	char buf[DATA_BUFFER];
	if (geteuid() == 0)
	{ // root
		write(fd, "LOGIN root", SIZE_BUFFER);
		puts("LOGIN root");
		strcpy(username, "root");
	}
	read(fd, buf, SIZE_BUFFER);
	puts(buf);
	return strcmp(buf, "Login success\n") == 0;
}

/**    SETUP    **/
void *handleInput(void *client_fd)
{
	int fd = *(int *)client_fd;
	char message[DATA_BUFFER] = {0};

	while (1)
	{
		if (wait)
			continue;
		printf("#A09 ");
		fgets(message, DATA_BUFFER, stdin);
		char *tmp = strtok(message, "\n");
		if (tmp != NULL)
		{
			strcpy(message, tmp);
		}
		if (strcmp(message, "exit") == 0)
		{
			exit(EXIT_SUCCESS);
		}
		send(fd, message, SIZE_BUFFER, 0);
		wait = true;
	}
}

void *handleOutput(void *client_fd)
{
	int fd = *(int *)client_fd;
	char message[DATA_BUFFER] = {0};

	while (1)
	{
		memset(message, 0, SIZE_BUFFER);
		getServerOutput(fd, message);
		printf("%s", message);
		fflush(stdout);
		wait = false;
	}
}

void getServerOutput(int fd, char *input)
{
	if (recv(fd, input, DATA_BUFFER, 0) == 0)
	{
		printf("Disconnected from server\n");
		exit(EXIT_SUCCESS);
	}
}

int create_tcp_client_socket()
{
	struct sockaddr_in address;
	int server_fd, ret_val;
	int opt = 1;
	struct hostent *local_host; /* need netdb.h for this */

	/* Step1: create a TCP socket */
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/* Let us initialize the server address structure */
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	local_host = gethostbyname("127.0.0.1");
	address.sin_addr = *((struct in_addr *)local_host->h_addr);

	/* Step2: connect to the TCP server socket */
	ret_val = connect(server_fd, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
	if (ret_val == -1)
	{
		exit(EXIT_FAILURE);
	}
	return server_fd;
}
