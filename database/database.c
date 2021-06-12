#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

//BUAT CHECK ROOT ATAU BUKAN
int checkSudo() {
	uid_t uid = getuid();
	if (uid != 0) {
		return 0; // Login as user
	} else {
		return 1; //Login as root
	}
}

//FUNGSI BUAT NGAMBIL SPECifi WORD DI STRING
void nthword(char *str, int num) {
	char ret[80];

	int i = 0, strnum = 0;
	char *prev;
	prev = NULL;

	while (*str != '\0') {
		if (*str != ' ') {
			if ((prev == NULL) || (*prev == ' '))
				strnum++;
			if (strnum == num) {
				ret[i] = *str;
				i++;
			}
			str++;
			prev = str - 1;
		}
		else if (*str == ' ') {
			str++;
			prev = str - 1;
		}
	}
	ret[i] = '\0';
	printf("%s \n", ret);
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char *createUserSuccess = "Create user success";
    char *createUserFail = "Create user fail";
    char *useDBSucces = "Using Database..";
    char *useDBFail = "Database not found";
    char *msg_kurang = "Pengurangan berhasil";
    char *salah = "Command or query not found";
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while(1) {
        char buffer[1024] = {0};
        valread = read(new_socket, buffer, 1024);    
	// char *buffer = "CREATE USER daffainfo IDENTIFIED BY daffainfo;";
        if (strstr(nthword(buffer, 1), "CREATE") == 0 && strstr(nthword(buffer, 1), "USER") == 0) {
		if (strstr(nthword(buffer, 4), "IDENTIFIED") == 0 && strstr(nthword(buffer, 5), "BY") == 0) {
			strtok(nthword(buffer, 6), ";");
			send(new_socket, createUserSuccess, strlen(createUserSuccess), 0);
		} else {
			send(new_socket, createUserFail, strlen(createUserFail), 0);
		}
	}
	// char *buffer = "USE DATABASE1;";
        else if (strstr(nthword(buffer, 1), "USE") == 0 && nthword(buffer, 2) != 0) {
		char string[100], cwd[PATH_MAX];
		char *dapos = strtok(nthwork(buffer, 2), ";");
		getcwd(cwd, sizeof(cwd)) //semisal di /home/daffainfo/fp/
		sprintf(string, "%s/databases/%s", cwd, dapos);
		if (string) { //Jika benar
			send(new_socket, useDBSuccess, strlen(useDBSuccess), 0);
		} else {
			send(new_socket, useDBFail, strlen(useDBFail), 0);
		}
	}
        else if(strcmp(buffer, "cek") == 0) {
            char str[100];
            sprintf(str, "%d", num);
            send(new_socket, str, strlen(str), 0);
            printf("Message sent\n");
        }
        else {
            send(new_socket, salah, strlen(salah), 0);
            printf("Message sent\n");
        }
    }
    return 0;
}


	//Buat create db
	// char *test = "CREATE DATABASE dapos;";
	if (strcmp(nthword(test, 1), "CREATE") == 0 && strcmp(nthword(test, 2), "DATABASE") == 0)
	{
		char *dapos = strtok(nthwork(test, 3), ";");
		chdir("/databases");
		mkdir(dapos);

		if (mkdir(dapos) == 0) //jika berhasil
		{
			//database dapos created
		}
		else
		{
			//database is exist
		}
	}
	return 0;

	//BUAT DROP ANEH"
	if (strcmp(nthword(test, 1), "DROP") == 0)
	{
		if (strcmp(nthword(test, 2), "DATABASE") == 0)
		{
			char *dapos = strtok(nthwork(test, 3), ";");
			chdir("/databases");
			rmdir(dapos);
			if (rmdir(dapos) == 0) //jika berhasil
			{
				//database dapos deleted
			}
			else
			{
				//database dpos not exist
			}
		}
		else if (strcmp(nthword(test, 2), "TABLES") == 0)
		{
			char *dapos = strtok(nthwork(test, 3), ";");
			//PAKE FUNGSI USE DULU BIAR PINDAH DB
			remove(tables1) if (remove(tables1) == 0) //jika berhasil
			{
				//table dapos deleted
			}
			else
			{
				//table dapos not exist
			}
		}
		//MASIH SALAH!!
		else if (strcmp(nthword(test, 2), "COLUMNS") == 0)
		{
			if (strcmp(nthword(test, 4), "FROM") == 0)
			{
				char *dapos = strtok(nthwork(test, 5), ";");

				if (remove(tables1) == 0) //jika berhasil
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
				//FROM WHUTT TABLES??
			}
		}
	}
