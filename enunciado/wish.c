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

            //Leemos por linea
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
                if (strcmp(word, "") != 0 && strcmp(word, "\t") != 0)
                {
                    arguments[i] = word;
                    arguments[i + 1] = NULL;
                }
                else
                {
                    i = i - 1;
                };
            };

            //Recorrer los comandos
            for (int i = 0; arguments[i] != NULL; i++)
            {

                //Separar las palabras de cada comando
                char *word2;
                char *word3;
                char *arguments2[100];

                //Quito los espacios
                for (int j = 0; (word2 = strsep(&arguments[i], " ")) != NULL; j++)
                {
                    //Quito las tabulaciones
                    for (int k = 0; (word3 = strsep(&word2, "\t")) != NULL; k++)
                    {
                        //Confirmar que no este vacia la palabra
                        if (strcmp(word3, "") != 0)
                        {
                            arguments2[j] = word3;
                            arguments2[j + 1] = NULL;
                            j = j + 1;
                        }
                    }
                    j = j - 1;
                };

                //Validamos si el comando es exit, cd o path
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

                //Recorremos la ruta que nos mandaron
                for (int j = 0; mypath[j] != NULL; j++)
                {
                    char *path = strdup(mypath[j]);
                    strcat(path, arguments2[0]);

                    //Validamos si el comando existe en la ruta
                    if (access(path, F_OK) == 0)
                    {
                        if (fork() == 0)
                        {
                            execv(path, arguments2);
                            return (0);
                        };
                        wait(NULL);
                        break;
                    };
                };
            };
        };
    };

    //Modo batch
    if (argc == 2)
    {
        //Leemos el archivo
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            printf("wunzip: cannot open file\n");
            exit(1);
        };

        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        //Leemos por linea del archivo
        while ((read = getline(&line, &len, fp)) != -1)
        {
            char c1[2];
            c1[0] = line[strlen(line) - 1];
            c1[1] = line[strlen(line) - 2];

            if (strcmp(c1, "\r\n") != 0)
            {
                line[strlen(line) - 1] = '\0';
                line[strlen(line) - 2] = '\0';
            }

            //Guardar cada comando en una matriz
            char *word;
            char *arguments[100];
            for (int i = 0; (word = strsep(&line, "&")) != NULL; i++)
            {
                if (strcmp(word, "") != 0 && strcmp(word, "\t") != 0)
                {
                    arguments[i] = word;
                    arguments[i + 1] = NULL;
                }
                else
                {
                    i = i - 1;
                };
            };

            //Recorrer los comandos
            for (int i = 0; arguments[i] != NULL; i++)
            {

                //Separar las palabras de cada comando
                char *word2;
                char *word3;
                char *arguments2[100];

                //Quito los espacios
                for (int j = 0; (word2 = strsep(&arguments[i], " ")) != NULL; j++)
                {
                    //Quito las tabulaciones
                    for (int k = 0; (word3 = strsep(&word2, "\t")) != NULL; k++)
                    {
                        //Confirmar que no este vacia la palabra
                        if (strcmp(word3, "") != 0)
                        {
                            arguments2[j] = word3;
                            arguments2[j + 1] = NULL;
                            j = j + 1;
                        }
                    }
                    j = j - 1;
                };

                //Validamos si el comando es exit, cd o path
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

                //Recorremos la ruta que nos mandaron
                for (int j = 0; mypath[j] != NULL; j++)
                {
                    char *path = strdup(mypath[j]);
                    strcat(path, arguments2[0]);

                    //Validamos si el comando existe en la ruta
                    if (access(path, F_OK) == 0)
                    {
                        if (fork() == 0)
                        {
                            execv(path, arguments2);
                            return (0);
                        };
                        wait(NULL);
                        break;
                    };
                };
            };
        };
    };

    //Mando algo erroneo
    write(STDERR_FILENO, error_message, strlen(error_message));
    return (0);
}
