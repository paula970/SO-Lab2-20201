
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

// const char *mypath[] = {
//   "./",
//   "/usr/bin/",
//   "/bin/",
//   NULL
// };

int main(int argc, char *argv[])
{
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
                    if (j == 0 && i == 0)
                    {
                        // EXIT
                        if (strcmp(word2, "exit") == 0)
                            {
                                exit(0);
                            };
                        // CD
                        if (strcmp(word2, "cd") == 0)
                        {
                            word2 = strsep(&arguments[i], " ");
                            if (chdir(word2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                            }
                            else
                            {
                                char s[100];
                                printf("Ubicación después de CD: %s\n", getcwd(s, 100));
                            }
                        };
                        //PATH
                        if (strcmp(word2, "path") == 0)
                        {
                            printf("path");
                        };
                    };
                    arguments2[i] = word2;
                    arguments2[i + 1] = NULL;
                };
            };

            /* If necessary locate executable using mypath array */
            /* Launch executable */
            // if (fork () == 0) {
            //     ...
            //     execv (...);
            //     ...
            // }
            // else
            // {
            //     wait (...);
            // }
        }
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
