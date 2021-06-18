#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#define PORT 8080
#define DATA_BUFFER 300

int curr_fd = -1;
int curr_id = -1;
const int SIZE_BUFFER = sizeof(char) * DATA_BUFFER;

// Socket setup
int create_tcp_server_socket();

// Routes & controller
void *routes(void *argv);
bool login(int fd, char *username, char *password);
void regist(int fd, char *username, char *password);

int main()
{
	socklen_t addrlen;
	struct sockaddr_in new_addr;
	pthread_t tid;
	char buf[DATA_BUFFER];
	int server_fd = create_tcp_server_socket();
	int new_fd;

	while (1)
	{
		new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
		if (new_fd >= 0)
		{
			pthread_create(&tid, NULL, &routes, (void *)&new_fd);
		}
	} /* while(1) */
	return 0;
}

void *routes(void *argv)
{
	int fd = *(int *)argv;
	char query[DATA_BUFFER], buf[DATA_BUFFER];

	while (read(fd, query, DATA_BUFFER) != 0)
	{
		puts(query);

		strcpy(buf, query);
		char *cmd = strtok(buf, " ");

		if (strcmp(cmd, "LOGIN") == 0)
		{
			char *username = strtok(NULL, " ");
			char *password = "root";
			if (!login(fd, username, password))
				break;
		}
		else if (strcmp(cmd, "CREATE") == 0)
		{
			cmd = strtok(NULL, " ");

			if (strcmp(cmd, "DATABASE") == 0)
			{
				//create db ngapain
			}
			else
			{
				write(fd, "Invalid query on CREATE command\n", SIZE_BUFFER);
			}
		}
		else
		{
			write(fd, "Invalid query\n", SIZE_BUFFER);
		}
	}
	if (fd == curr_fd)
	{
		curr_fd = curr_id = -1;
	}
	printf("Close connection with fd: %d\n", fd);
	close(fd);
}

/****   Controllers   *****/

bool login(int fd, char *username, char *password)
{
	if (curr_fd != -1)
	{
		write(fd, "Server is busy, wait for other user to logout.\n", SIZE_BUFFER);
		return false;
	}

	int id = -1;
	if (strcmp(username, "root") == 0)
	{
		id = 0;
	}

	if (id == -1)
	{
		write(fd, "Error::Invalid id or password\n", SIZE_BUFFER);
		return false;
	}
	else
	{
		write(fd, "Login success\n", SIZE_BUFFER);
		curr_fd = fd;
		curr_id = id;
	}
	return true;
}

/****   SOCKET SETUP    *****/
int create_tcp_server_socket()
{
	struct sockaddr_in address;
	int fd, ret_val;
	int opt = 1;

	/* Step1: create a TCP socket */
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1)
	{
		fprintf(stderr, "socket failed [%s]\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/* Initialize the socket address structure */
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	/* Step2: bind the socket to port 7000 on the local host */
	ret_val = bind(fd, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
	if (ret_val != 0)
	{
		fprintf(stderr, "bind failed [%s]\n", strerror(errno));
		close(fd);
		exit(EXIT_FAILURE);
	}

	/* Step3: listen for incoming connections */
	ret_val = listen(fd, 5);
	if (ret_val != 0)
	{
		fprintf(stderr, "listen failed [%s]\n", strerror(errno));
		close(fd);
		exit(EXIT_FAILURE);
	}
	return fd;
}
