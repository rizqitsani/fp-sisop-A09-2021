#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <limits.h>
#define PORT 8080

int curr_fd = -1, curr_id = -1;
const int SIZE_BUFFER = sizeof(char) * 1000;

void *routes(void *argv);
bool login(int fd, char *username, char *password);

int main()
{
	struct sockaddr_in address, new_addr;
	int fd, new_fd, ret_val, opt = 1;
	socklen_t addrlen;
	pthread_t tid;
	char buf[1000];

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

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	ret_val = bind(fd, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
	if (ret_val != 0)
	{
		fprintf(stderr, "bind failed");
		close(fd);
		exit(EXIT_FAILURE);
	}

	ret_val = listen(fd, 5);
	if (ret_val != 0)
	{
		fprintf(stderr, "listen failed");
		close(fd);
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		new_fd = accept(fd, (struct sockaddr *)&new_addr, &addrlen);
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
	char query[1000], buf[1000];
	char path[1000];

	while (read(fd, query, 1000) != 0)
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
			char coba[100];
			cmd = strtok(NULL, " ");
			strcpy(coba, cmd);

			if (strcmp(cmd, "DATABASE") == 0)
			{
				char string[5000], cwd[PATH_MAX];
				char *token = strtok(NULL, " ");

				if (getcwd(cwd, sizeof(cwd)) != NULL)
				{
					char *token2 = strtok(token, ";");
					sprintf(string, "%s/databases/%s", cwd, token2);
					int check = mkdir(string, 0777);
					if (!check)
					{
						write(fd, "Create DB Success\n", SIZE_BUFFER);
					}
					else
					{
						write(fd, "Create DB Failed\n", SIZE_BUFFER);
					}
				}
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
			char string[5000], cwd[PATH_MAX];

			if (getcwd(cwd, sizeof(cwd)) != NULL)
			{
				char temp[5000];
				char *token2 = strtok(cmd, ";");
				sprintf(string, "%s/databases/%s", cwd, cmd);
				strcpy(temp, string);
				strcpy(path, string);
				memset(string, 0, 5000);
				printf("%s\n", temp);
				int dapos = chdir(temp);
				printf("%d", dapos);
				if (dapos == 0)
				{
					write(fd, "Pindah DB Success\n", SIZE_BUFFER);
				}
				else
				{
					write(fd, "Pindah DB Failed\n", SIZE_BUFFER);
				}
				// memset(string, 0, 5000);
				// printf("%s\n",string);
			}
		}

		else if (strcmp(cmd, "DROP") == 0)
		{
			cmd = strtok(NULL, " ");

			if (strcmp(cmd, "DATABASE") == 0)
			{
				char string[5000], cwd[PATH_MAX];
				char *token = strtok(NULL, " ");

				if (getcwd(cwd, sizeof(cwd)) != NULL)
				{
					char *token2 = strtok(token, ";");
					sprintf(string, "%s/databases/%s", cwd, token2);
					int check = rmdir(string);
					if (!check)
					{
						write(fd, "Drop database success\n", SIZE_BUFFER);
					}
					else
					{
						write(fd, "Drop database failed\n", SIZE_BUFFER);
					}
				}
				//create db ngapain
			}
			else if (strcmp(cmd, "TABLE") == 0)
			{
				char string[5000], cwd[PATH_MAX];
				char *token = strtok(NULL, " ");

				if (getcwd(cwd, sizeof(cwd)) != NULL)
				{
					char *token2 = strtok(token, ";");
					sprintf(string, "%s/databases/%s", cwd, token2);
					int check = remove(string);
					if (!check)
					{
						write(fd, "Remove table success\n", SIZE_BUFFER);
					}
					else
					{
						write(fd, "Remove table failed\n", SIZE_BUFFER);
					}
				}
				//drop db ngapain
			}
			else
			{
				write(fd, "drop gagal\n", SIZE_BUFFER);
			}
		}
		else if (strcmp(cmd, "DELETE") == 0)
		{
			cmd = strtok(NULL, " ");

			if (strcmp(cmd, "FROM") == 0)
			{
				char string[5000], cwd[PATH_MAX];
				char *token = strtok(NULL, " ");

				if (getcwd(cwd, sizeof(cwd)) != NULL)
				{
					FILE *data;
					char *token2 = strtok(token, ";");
					sprintf(string, "%s/databases/%s", cwd, token2);
					data = fopen(string, "w");
					if (data)
					{
						fclose(data);
					}
				}
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
	close(fd);
}

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
