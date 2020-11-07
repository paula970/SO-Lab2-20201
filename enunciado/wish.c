#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *mypath[] = {
        "./",
        "/usr/bin/",
        "/bin/",
        NULL};

    char error_message[30] = "An error has occurred\n";

    //Modo interactivo
    if (argc == 1)
    {

        while (1)
        {
            //Wait for input
            printf("prompt> ");
            char *line = NULL;
            size_t len = 0;
            ssize_t read;
            if ((read = getline(&line, &len, stdin)) == -1)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                continue;
            }

            //Eliminar espacio al final
            line[strlen(line) - 1] = '\0';

            //Guardar cada comando en una matriz
            char *word;
            char *arguments[100];
            for (int i = 0; (word = strsep(&line, "&")) != NULL; i++)
            {
                arguments[i] = word;
                arguments[i + 1] = NULL;
            };

            //Recorrer los comandos
            for (int i = 0; arguments[i] != NULL; i++)
            {

                //Separar las palabras de cada comando
                char *word2;
                char *arguments2[100];
                for (int j = 0; (word2 = strsep(&arguments[i], " ")) != NULL; j++)
                {

                    arguments2[j] = word2;
                    arguments2[j + 1] = NULL;
                };
                if (i == 0)
                {

                    // EXIT
                    if (strcmp(arguments2[0], "exit") == 0)
                    {
                        exit(0);
                        break;
                    };
                    // CD
                    if (strcmp(arguments2[0], "cd") == 0)
                    {

                        if (chdir(arguments2[1]) == -1)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }
                        else
                        {
                            char s[100];
                            printf("Ubicación después de CD: %s\n", getcwd(s, 100));
                        }
                        break;
                    };
                    //PATH
                    if (strcmp(arguments2[0], "path") == 0)
                    {

                        for (int k = 1; arguments2[k] != NULL; k++)
                        {
                            mypath[k - 1] = arguments2[k];
                            mypath[k] = NULL;
                        }

                        break;
                    };
                };

                for (int j = 0; mypath[j] != NULL; j++)
                {
                    char *path = strdup(mypath[j]);
                    strcat(path, arguments2[0]);

                    if (access(path, F_OK) == 0)
                    {
                        printf("%s \n", path);
                        if(fork()==0){
                            execv(path,arguments2);
                            return (0);
                        };
                        wait(NULL);
                        break;
                    };
                };
            };
        };
    };

    /* Modo batch */
    if (argc == 2)
    {
        printf("holi");
    };

    /* Mando algo erroneo */
    write(STDERR_FILENO, error_message, strlen(error_message));
    return (0);
}
