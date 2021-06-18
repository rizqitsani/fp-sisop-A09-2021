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
void logging(char *string);
bool login(int fd, char *username, char *password);
bool isFileExists(char *filename);

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
		logging(buf);
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
				char string[5000], cwd[PATH_MAX], tempDir[PATH_MAX], *checkDir;
				char *token = strtok(NULL, " ");

				getcwd(cwd, sizeof(cwd));
				strcpy(tempDir, cwd);

				printf("tempdir: %s\n", tempDir);

				// Kembali ke parent kalau lagi use db
				checkDir = strstr(cwd, "databases/");
				if (checkDir != NULL)
				{
					chdir("../../");
				}

				if (getcwd(cwd, sizeof(cwd)) != NULL)
				{
					char *token2 = strtok(token, ";");
					sprintf(string, "%s/databases/%s", cwd, token2);
					int check = mkdir(string, 0777);
					if (!check)
					{
						write(fd, "Create DB Success\n", SIZE_BUFFER);

						// Dikembalikan ke working directory semula
						chdir(tempDir);
					}
					else
					{
						write(fd, "Create DB Failed\n", SIZE_BUFFER);
					}
				}
			}
			else if (strcmp(cmd, "TABLE") == 0)
			{
				char fileDir[5000], cwd[PATH_MAX];
				char *tableName = strtok(NULL, " ");

				printf("table: %s\n", tableName);

				if (getcwd(cwd, sizeof(cwd)) != NULL)
				{
					FILE *data;
					char *columnDetail = strtok(NULL, "()");

					printf("column: %s\n", columnDetail);
					sprintf(fileDir, "%s/%s", cwd, tableName);

					printf("dir: %s\n", fileDir);

					if (isFileExists(fileDir))
					{
						write(fd, "Tabel sudah dibuat sebelumnya!\n", SIZE_BUFFER);
					}
					else
					{
						data = fopen(fileDir, "a");

						fprintf(data, "%s\n", columnDetail);
						fclose(data);
						write(fd, "Tabel berhasil dibuat!\n", SIZE_BUFFER);
					}
				}
			}
			else
			{
				write(fd, "Invalid query on CREATE command\n", SIZE_BUFFER);
			}
		}
		else if (strcmp(cmd, "USE") == 0)
		{
			char cwd[PATH_MAX], dbDir[5000], *checkDir;
			cmd = strtok(NULL, " ");

			getcwd(cwd, sizeof(cwd));

			checkDir = strstr(cwd, "databases");
			if (checkDir != NULL)
			{
				chdir("../../");
			}

			if (getcwd(cwd, sizeof(cwd)) != NULL)
			{
				char temp[5000];
				char *dbName = strtok(cmd, ";");

				printf("%s\n", dbName);
				printf("cwd:%s\ncmd:%s\n", cwd, cmd);
				sprintf(dbDir, "%s/databases/%s", cwd, cmd);
				printf("dir: %s\n", dbDir);

				int status = chdir(dbDir);
				printf("status: %d\n", status);

				if (status == 0)
				{
					write(fd, "Pindah DB Success\n", SIZE_BUFFER);
				}
				else
				{
					write(fd, "Pindah DB Failed\n", SIZE_BUFFER);
				}
			}
		}
		else if (strcmp(cmd, "DROP") == 0)
		{
			cmd = strtok(NULL, " ");

			if (strcmp(cmd, "DATABASE") == 0)
			{
				char string[5000], cwd[PATH_MAX], tempDir[PATH_MAX], *checkDir;
				char *token = strtok(NULL, " ");

				getcwd(cwd, sizeof(cwd));
				strcpy(tempDir, cwd);

				printf("tempdir: %s\n", tempDir);

				// Kembali ke parent kalau lagi use
				checkDir = strstr(cwd, "databases/");
				if (checkDir != NULL)
				{
					chdir("../../");
				}

				if (getcwd(cwd, sizeof(cwd)) != NULL)
				{
					char *token2 = strtok(token, ";");
					sprintf(string, "%s/databases/%s", cwd, token2);
					int check = rmdir(string);
					if (!check)
					{
						write(fd, "Drop database success\n", SIZE_BUFFER);

						// Dikembalikan ke working directory semula
						chdir(tempDir);
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
					sprintf(string, "%s/%s", cwd, token2);
					printf("%s\n", string);
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
					sprintf(string, "%s/%s", cwd, token2);
					printf("%s\n", string);
					data = fopen(string, "w");
					if (data)
					{
						write(fd, "Delete data success\n", SIZE_BUFFER);
						fclose(data);
					}
					else
					{
						write(fd, "Delete data failed\n", SIZE_BUFFER);
					}
				}
			}
		}
		else if (strcmp(cmd, "INSERT") == 0)
		{
			cmd = strtok(NULL, " ");

			if (strcmp(cmd, "INTO") == 0)
			{
				char fileDir[5000], cwd[PATH_MAX];
				char *tableName = strtok(NULL, " ");

				printf("table: %s\n", tableName);

				if (getcwd(cwd, sizeof(cwd)) != NULL)
				{
					FILE *data;
					char *value = strtok(NULL, "()");

					printf("value: %s\n", value);
					sprintf(fileDir, "%s/%s", cwd, tableName);

					printf("dir: %s\n", fileDir);

					if (isFileExists(fileDir))
					{
						data = fopen(fileDir, "a");

						fprintf(data, "%s\n", value);
						fclose(data);
						write(fd, "Data berhasil dimasukkan!\n", SIZE_BUFFER);
					}
					else
					{
						write(fd, "Tabel tidak ditemukan!\n", SIZE_BUFFER);
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

bool isFileExists(char *filename)
{
	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}

void logging(char *str)
{
	FILE *fp = fopen("/home/rizqitsani/SinSeiFS.log", "a+");

	time_t rawtime;
	struct tm *info;
	char buffer[1000];

	time(&rawtime);

	info = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", info);
	fprintf(fp, "%s:root:%s\n", buffer, str);
	fclose(fp);
}
