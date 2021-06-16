#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    DIR *dp;
    FILE *fp;
    struct dirent *ep;
    char wkwk[100], string[100], cwd[PATH_MAX];
    char *dapos = strtok(nthword(buffer, 2), ";");
    getcwd(cwd, sizeof(cwd)) //semisal di /home/daffainfo/fp/
        strcat(wkwk, nthword(buffer, 5));
    sprintf(string, "%s/databases/%s", cwd, dapos, wkwk);
    chdir(string) //ini adalah /home/daffainfo/fp/databases/nama_db/

        dp = opendir(string);

    if (dp != NULL)
    {
        while ((ep = readdir(dp)))
        {
            char table[100], char fileText[100];
            fp = fopen(ep->d_name, "r");

            fgets(fileText, 100, fp);
            while ((holder = fgetc(fp)) != EOF)
            {
                if (holder == '\n')
                    line++;
                if (line == 1)
                    break; /* 1 because count start from 0,you know */
            }
            printf("DROP TABLE %s;", ep->d_name);
            printf("CREATE TABLE %s (%s);", ep->d_name, fileText);

            while ((holder = fgetc(fp)) != EOF && (holder != '\n'))
            {
                printf("INSERT INTO %s (%s);", ep->d_name, holder);
            }

            fclose(fp);
        }

        (void)closedir(dp);
    }
    else
        perror("Couldn't open the directory");

    return 0;
}
