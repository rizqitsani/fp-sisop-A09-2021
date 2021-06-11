//Masih logika sama error handling gaes
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//BUAT CHECK ROOT ATAU BUKAN
int checkSudo()
{
	uid_t uid = getuid(), euid = geteuid();
	if (uid != 0)
	{
		return 0; // Login as user
	}
	else
	{
		return 1; //Login as root
	}
}

//Setelah login

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

int main()
{
	//Check login
	if (strcmp(argv[1], "-u") == 0 && argv[2] != 0)
	{
		if (strcmp(argv[3], "-p") == 0 && argv[4] != 0)
		{
			if (user & pass ada)
			{
				//login
			}
			else
			{
				// user / password salah
			}
		}
		else
		{
			//echo "Butuh password"
		}
	}
	else
	{
		//echo "Welcome, to use this program add argument -u for user and -p for password"
	}

	//Buat create user ini
	// char *test = "CREATE USER jack IDENTIFIED BY jack123;";
	if (strcmp(nthword(test, 1), "CREATE") == 0 && strcmp(nthword(test, 2), "USER") == 0)
	{
		if (strcmp(nthword(test, 4), "IDENTIFIED") == 0 && strcmp(nthword(test, 5), "BY") == 0)
		{
			strtok(nthwork(test, 6), ";");
			// echo create user sucess
		}
		else
		{
			// password yang bener
		}
	}
	else
	{
		//user yang bener
	}

	//Buat use db
	// char *test = "USE DATABASE1;";
	if (strcmp(nthword(test, 1), "USE") == 0 && nthword(test, 2) != 0)
	{
		char *dapos = strtok(nthwork(test, 2), ";");
		getcwd() //semisal di /home/daffainfo/fp/

			if (chdir("/databases/${dapos}") == 0) //jika benar
		{
			//database dapos used
		}
		else
		{
			//database not found
		}
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
			remove(tables1)
			if (remove(tables1) == 0) //jika berhasil
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
