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

int curr_fd = -1;
int curr_id = -1;
const int SIZE_BUFFER = sizeof(char) * 300;

// Socket setup
int create_tcp_server_socket();

// Routes & controller
void *routes(void *argv);
bool login(int fd, char *username, char *password);

int main()
{
	socklen_t addrlen;
	struct sockaddr_in new_addr;
	pthread_t tid;
	char buf[300];
	int server_fd = create_tcp_server_socket();
	int new_fd;

	while (1)
	{
		new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
		if (new_fd >= 0)
		{
			pthread_create(&tid, NULL, &routes, (void *)&new_fd);
		}
	}
	return 0;
}

void *routes(void *argv)
{
	int fd = *(int *)argv;
	char query[300], buf[300];

	while (read(fd, query, 300) != 0)
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
				cmd = strtok(NULL, " ");
				//create db ngapain
			}
			else if (strcmp(cmd, "TABLE") == 0)
			{
				cmd = strtok(NULL, " ");
				//create table ngapain
			}
			else
			{
				write(fd, "Invalid query on CREATE command\n", SIZE_BUFFER);
			}
		}
		else if (strcmp(cmd, "USE") == 0)
		{
			cmd = strtok(NULL, " ");

			//Use database ngapain
		}

		else if (strcmp(cmd, "DROP") == 0)
		{
			cmd = strtok(NULL, " ");

			if (strcmp(cmd, "DATABASE") == 0)
			{
				cmd = strtok(NULL, " ");
				//drop db ngapain
			}
			else if (strcmp(cmd, "TABLE") == 0)
			{
				cmd = strtok(NULL, " ");
				//drop db ngapain
			}
			else
			{
				write(fd, "drop gagal\n", SIZE_BUFFER);
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
	int id = -1;
	if (curr_fd != -1)
	{
		write(fd, "Server is busy\n", SIZE_BUFFER);
		return false;
	}

	if (strcmp(username, "root") == 0)
	{
		id = 0;
	}

	if (id != -1)
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
		fprintf(stderr, "bind failed");
		close(fd);
		exit(EXIT_FAILURE);
	}

	/* Step3: listen for incoming connections */
	ret_val = listen(fd, 5);
	if (ret_val != 0)
	{
		fprintf(stderr, "listen failed");
		close(fd);
		exit(EXIT_FAILURE);
	}
	return fd;
}
