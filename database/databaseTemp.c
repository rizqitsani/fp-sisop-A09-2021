#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PORT 8080

void tostring(char str[], int num);

int main(int argc, char const *argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char *createDB = "Create database exists";
	char *createDBFail = "Database is exist";
	char *salah = "Query salah";

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

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		char query[5555];
		char buffer[1024] = {0};
		valread = read(new_socket, buffer, 1024);
		strcpy(query, buffer);
		if (strcmp(query, "USE") == 0)
		{
			send(new_socket, "cpk", strlen("cpk"), 0);
			printf("Message sent\n");
		}

		else if (strstr(query, "CREATE DATABASE") != NULL)
		{
			char string[100], cwd[PATH_MAX];
			char *token, *token2;
			int check;
			token = strtok(query, "CREATE DATABASE ");
			token2 = strtok(token, ";");
			if (getcwd(cwd, sizeof(cwd)) != NULL)
			{
				sprintf(string, "%s/databases/%s", cwd, token2);
				check = mkdir(string, 0777);
				if (!check)
				{
					send(new_socket, createDB, strlen(createDB), 0);
				}
				else
				{
					send(new_socket, createDBFail, strlen(createDBFail), 0);
				}
			}
		}
		else
		{
			send(new_socket, salah, strlen(salah), 0);
		}
	}
	return 0;
}
