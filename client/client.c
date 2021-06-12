#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int checkSudo()
{
	uid_t uid = getuid();
	if (uid != 0)
	{
		return 0; // Login as user
	}
	else
	{
		return 1; //Login as root
	}
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in address;
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}

	if (checkSudo() == 1)
	{
		while (1)
		{
			char buffer[1024] = {0};
			char *str;
			printf("Welcome to A09 Database, Please Input Query");
			scanf("%s", str);
			if (strcmp(str, "exit") == 0)
			{
				exit(0);
			}
			send(sock, str, strlen(str), 0);
			printf("Query Executed\n");
			valread = read(sock, buffer, 1024);
			printf("%s\n", buffer);
		}
	}
	if (strcmp(argv[1], "-u") == 0 && argv[2] != 0 && strcmp(checkSudo(), 1) == 0)
	{
		if (strcmp(argv[3], "-p") == 0 && argv[4] != 0)
		{
			if (user & pass ada)
			{
				while (1)
				{
					char buffer[1024] = {0};
					char *str;
					printf("Welcome to A09 Database, Please Input Query");
					scanf("%s", str);
					if (strcmp(str, "exit") == 0)
					{
						exit(0);
					}
					else if (strstr(str, "CREATE USER") == 0)
					{
						str = '\0';
						printf("Normal user can't create user!";
					}
					else if (strstr(str, "GRANT PERMISSION") == 0)
					{
						str = '\0';
						printf("Normal user can't grant permission!";
					}
					send(sock, str, strlen(str), 0);
					printf("Query Executed\n");
					valread = read(sock, buffer, 1024);
					printf("%s\n", buffer);
				}
			}
			else
			{
				printf("User atau password salah");
			}
		}
		else
		{
			printf("Butuh password");
		}
	}
	else
	{
		printf("Selamat datang di database A09");
	}
	return 0;
}
