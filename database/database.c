#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

//BUAT CHECK ROOT ATAU BUKAN
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

//FUNGSI BUAT NGAMBIL SPECifi WORD DI STRING
void nthword(char *str, int num)
{
	char ret[80];

	int i = 0, strnum = 0;
	char *prev;
	prev = NULL;

	while (*str != '\0')
	{
		if (*str != ' ')
		{
			if ((prev == NULL) || (*prev == ' '))
				strnum++;
			if (strnum == num)
			{
				ret[i] = *str;
				i++;
			}
			str++;
			prev = str - 1;
		}
		else if (*str == ' ')
		{
			str++;
			prev = str - 1;
		}
	}
	ret[i] = '\0';
	printf("%s \n", ret);
}

int main(int argc, char const *argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char *createUser = "Create user success";
	char *createUserFail = "Create user fail";
	char *createDB = "Create database exists";
	char *createDBFail = "Database is exist";
	char *useDB = "Using Database..";
	char *useDBFail = "Database not found";
	char *dropDatabase = "Database dropped";
	char *dropDatabaseFail = "Database not found";
	char *dropTable = "Table dropped";
	char *dropTableFail = "Table not found";
	char *dropColumn = "Table dropped";
	char *dropColumnFail = "Table not found";
	char *askTable = "Table dropped";
	char *salah = "Command or query not found";

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
		char buffer[1024] = {0};
		valread = read(new_socket, buffer, 1024);
		// char *buffer = "CREATE USER daffainfo IDENTIFIED BY daffainfo;";
		if (strcmp(nthword(buffer, 1), "CREATE") == 0 && strcmp(nthword(buffer, 1), "USER") == 0)
		{
			if (strcmp(nthword(buffer, 4), "IDENTIFIED") == 0 && strcmp(nthword(buffer, 5), "BY") == 0)
			{
				strtok(nthword(buffer, 6), ";");
				send(new_socket, createUser, strlen(createUser), 0);
			}
			else
			{
				send(new_socket, createUserFail, strlen(createUserFail), 0);
			}
		}
		// char *buffer = "USE DATABASE1;";
		else if (strcmp(nthword(buffer, 1), "USE") == 0 && nthword(buffer, 2) != 0)
		{
			char string[100], cwd[PATH_MAX];
			char *dapos = strtok(nthword(buffer, 2), ";");
			getcwd(cwd, sizeof(cwd)) //semisal di /home/daffainfo/fp/
				sprintf(string, "%s/databases/%s", cwd, dapos);
			if (chdir(string) != NULL)
			{ //Jika benar
				send(new_socket, useDB, strlen(useDB), 0);
			}
			else
			{
				send(new_socket, useDBFail, strlen(useDBFail), 0);
			}
		}
		// char *buffer = "CREATE DATABASE daffainfo;";
		else if (strcmp(nthword(buffer, 1), "CREATE") == 0 && strcmp(nthword(buffer, 2), "DATABASE") == 0)
		{
			char string[100], cwd[PATH_MAX];
			char *dapos = strtok(nthword(buffer, 3), ";");
			getcwd(cwd, sizeof(cwd)) //semisal di /home/daffainfo/fp/
				sprintf(string, "%s/databases/%s", cwd, dapos);
			if (mkdir(string) == 0)
			{
				send(new_socket, createDB, strlen(createDB), 0);
			}
			else
			{
				send(new_socket, createDBFail, strlen(createDBFail), 0);
			}
		}
		// char *buffer = "DROP [DATABASE | TABLE | COLUMN] [nama_database | nama_tabel | [nama_kolom] FROM [nama_tabel]];";
		//BUAT DROP ANEH"
		else if (strcmp(nthword(buffer, 1), "DROP") == 0)
		{
			if (strcmp(nthword(buffer, 2), "DATABASE") == 0)
			{
				char *dapos = strtok(nthword(buffer, 3), ";");
				char string[100], cwd[PATH_MAX];
				getcwd(cwd, sizeof(cwd)) //semisal di /home/daffainfo/fp/
					sprintf(string, "%s/databases/", cwd);
				chdir(string);
				rmdir(dapos);
				if (rmdir(dapos) == 0)
				{
					send(new_socket, dropDatabase, strlen(dropDatabase), 0);
				}
				else
				{
					send(new_socket, dropDatabaseFail, strlen(dropDatabaseFail), 0);
				}
			}
			else if (strcmp(nthword(buffer, 2), "TABLES") == 0)
			{
				char string[100], cwd[PATH_MAX], tables[100];
				char *dapos = strtok(nthword(buffer, 2), ";");
				getcwd(cwd, sizeof(cwd)) //semisal di /home/daffainfo/fp/
					sprintf(string, "%s/databases/%s", cwd, dapos);
				strcat(tables, nthword(buffer, 3));
				remove(tables);
				if (remove(tables) == 0)
				{
					send(new_socket, dropTable, strlen(dropTable), 0);
				}
				else
				{
					send(new_socket, dropTableFail, strlen(dropTableFail), 0);
				}
			}
			//MASIH SALAH!!
			else if (strcmp(nthword(buffer, 2), "COLUMNS") == 0)
			{
				if (strcmp(nthword(buffer, 4), "FROM") == 0)
				{
					char *dapos = strtok(nthword(buffer, 5), ";");

					if (remove(tables1) == 0)
					{
						//column dapos deleted
					}
					else
					{
						//column dapos not exist
					}
				}
				else
				{
					send(new_socket, "Input the table too!", strlen("Input the table too!"), 0);
				}
			}
		}
		// char *buffer = DELETE FROM [nama_tabel];
		else if (strcmp(nthword(buffer, 1), "DELETE") == 0 && strcmp(nthword(buffer, 2), "FROM") == 0)
		{
			char string[100], cwd[PATH_MAX];
			FILE *fp;
			char *dapos = strtok(nthword(buffer, 3), ";");
			getcwd(cwd, sizeof(cwd)) //semisal di /home/daffainfo/fp/
				sprintf(string, "%s/databases/%s", cwd, dapos);

			fp = fopen("file.txt", "w+");
			if (mkdir(string) == 0)
			{
				send(new_socket, createDB, strlen(createDB), 0);
			}
			else
			{
				send(new_socket, createDBFail, strlen(createDBFail), 0);
			}
		}
		else
		{
			send(new_socket, salah, strlen(salah), 0);
			printf("Message sent\n");
		}
	}
	return 0;
}
