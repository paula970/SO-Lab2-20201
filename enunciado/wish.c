#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    char **mypath;
    char *bin = "/bin/";
    mypath = (char **)malloc(3 * sizeof(char *));
    mypath[0] = bin;
    mypath[1] = NULL;

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
            char *extra = strdup(line);
            char *palabraAux;
            char *palabraAux2;
            char *palabraAux3;

            int alguno = 0;

            for (int j = 0; (palabraAux = strsep(&extra, " ")) != NULL; j++)
            {
                for (int k = 0; (palabraAux2 = strsep(&palabraAux, "\t")) != NULL; k++)
                {
                    for (int k = 0; (palabraAux3 = strsep(&palabraAux2, "&")) != NULL; k++)
                    {
                        if (strcmp(palabraAux3, "") != 0)
                        {
                            alguno = 1;
                        }
                    }
                }
            }
            if (alguno == 0)
            {
                continue;
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
                char *file = NULL;
                char *file2 = NULL;
                int isRedirection = 0;
                int error = 0;
                char *args = strdup(arguments[i]);
                for (int j = 0; (word2 = strsep(&args, ">")) != NULL; j++)
                {
                    if (j == 0)
                    {
                        if (strcmp(word2, "") != 0)
                        {
                            arguments[i] = word2;
                        }
                        else
                        {
                            error = 1;
                        }
                    }
                    else if (j == 1)
                    {
                        isRedirection = 1;
                        file = word2;
                        if (file == NULL)
                        {
                            error = 1;
                        }
                    }
                    else
                    {
                        error = 1;
                        break;
                    }
                }
                if (isRedirection == 1)
                {
                    if (file != NULL)
                    {
                        int escrito = 0;
                        char *word6;
                        char *word7;

                        for (int j = 0; (word6 = strsep(&file, " ")) != NULL; j++)
                        {
                            for (int k = 0; (word7 = strsep(&word6, "\t")) != NULL; k++)
                            {
                                if (strcmp(word7, "") != 0)
                                {

                                    if (escrito == 0)
                                    {

                                        escrito = 1;
                                        file2 = word7;
                                        j = j + 1;
                                    }
                                    else
                                    {
                                        error = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if (error == 1 || (file2 == NULL && isRedirection == 1))
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    break;
                }

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
                        if (arguments2[1] == NULL)
                        {
                            exit(0);
                            break;
                        }
                        else
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            break;
                        }
                    };
                    // CD
                    if (strcmp(arguments2[0], "cd") == 0)
                    {

                        if (chdir(arguments2[1]) == -1)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }

                        break;
                    };
                    //PATH
                    if (strcmp(arguments2[0], "path") == 0)
                    {
                        if (arguments2[1] == NULL)
                        {
                            mypath[0] = NULL;
                        }
                        free(mypath);
                        mypath = (char **)malloc(sizeof(char *));
                        for (int k = 1; arguments2[k] != NULL; k++)
                        {
                            char *palabra = strdup(arguments2[k]);
                            char pathAct[200] = "./";
                            char pathAct2[200] = "";
                            char *auxxx;
                            char *slash = "/";
                            if (palabra[0] == 47)
                            {
                                strcat(pathAct2, arguments2[k]);
                                strcat(pathAct2, slash);
                                auxxx = strdup(pathAct2);
                            }
                            else
                            {
                                strcat(pathAct, arguments2[k]);
                                strcat(pathAct, slash);
                                auxxx = strdup(pathAct);
                            }
                            mypath[k - 1] = strdup(auxxx);
                            mypath[k] = NULL;
                        }

                        break;
                    };
                };
                int a = 1;
                //Recorremos la ruta que nos mandaron
                for (int j = 0; mypath[j] != NULL; j++)
                {
                    char *path = strdup(mypath[j]);
                    strcat(path, arguments2[0]);
                    if (access(path, F_OK) == 0)
                    {
                        if (isRedirection == 1)
                        {

                            a = 0;
                            if (fork() == 0)
                            {
                                close(STDOUT_FILENO);
                                open(file2, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
                                execvp(path, arguments2);
                                return (0);
                            }
                        }
                        else
                        {

                            a = 0;
                            if (fork() == 0)
                            {
                                execv(path, arguments2);
                                return (0);
                            };
                        }
                        wait(NULL);
                        break;
                    }

                    //Validamos si el comando existe en la ruta
                };
                if (a == 1)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
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
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        };
        FILE *fp2 = fopen(argv[1], "r");

        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        //Leemos por linea del archivo
        while ((read = getline(&line, &len, fp)) != -1)
        {
            int forks[100];
            forks[0] = 777;
            if (strcmp(line, "") != 0)
            {

                line[strlen(line) - 1] = '\0';
                char *extra = strdup(line);
                char *palabraAux;
                char *palabraAux2;
                char *palabraAux3;

                int alguno = 0;

                for (int j = 0; (palabraAux = strsep(&extra, " ")) != NULL; j++)
                {
                    for (int k = 0; (palabraAux2 = strsep(&palabraAux, "\t")) != NULL; k++)
                    {
                        for (int k = 0; (palabraAux3 = strsep(&palabraAux2, "&")) != NULL; k++)
                        {
                            if (strcmp(palabraAux3, "") != 0)
                            {
                                alguno = 1;
                            }
                        }
                    }
                }
                if (alguno == 0)
                {
                    continue;
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
                    char *file = NULL;
                    char *file2 = NULL;
                    int isRedirection = 0;
                    int error = 0;
                    char *args = strdup(arguments[i]);
                    //Separa redireccion y valida que si este bien, que no hayan archivos de mas y si se ingrese un comando inicial
                    for (int j = 0; (word2 = strsep(&args, ">")) != NULL; j++)
                    {
                        if (j == 0)
                        {
                            if (strcmp(word2, "") != 0)
                            {
                                arguments[i] = word2;
                            }
                            else
                            {
                                error = 1;
                            }
                        }
                        else if (j == 1)
                        {
                            isRedirection = 1;
                            file = word2;
                            if (file == NULL)
                            {
                                error = 1;
                            }
                        }
                        else
                        {
                            error = 1;
                            break;
                        }
                    }
                    //En caso de que sea redireccion valida que el archivo se encuentre bien organizado
                    if (isRedirection == 1)
                    {
                        if (file != NULL)
                        {
                            int escrito = 0;
                            char *word6;
                            char *word7;

                            for (int j = 0; (word6 = strsep(&file, " ")) != NULL; j++)
                            {
                                for (int k = 0; (word7 = strsep(&word6, "\t")) != NULL; k++)
                                {
                                    if (strcmp(word7, "") != 0)
                                    {

                                        if (escrito == 0)
                                        {

                                            escrito = 1;
                                            file2 = word7;
                                            j = j + 1;
                                        }
                                        else
                                        {
                                            error = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    //validan que no haya errores y exista un archivo en caso de que sea redireccion
                    if (error == 1 || (file2 == NULL && isRedirection == 1))
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        break;
                    }
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
                            if (arguments2[1] == NULL)
                            {
                                exit(0);
                                break;
                            }
                            else
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                break;
                            }
                        };
                        // CD
                        if (strcmp(arguments2[0], "cd") == 0)
                        {
                            if (chdir(arguments2[1]) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                            }

                            break;
                        };
                        //PATH
                        if (strcmp(arguments2[0], "path") == 0)
                        {
                            if (arguments2[1] == NULL)
                            {
                                mypath[0] = NULL;
                            }
                            free(mypath);
                            mypath = (char **)malloc(sizeof(char *));

                            for (int k = 1; arguments2[k] != NULL; k++)
                            {
                                char *palabra = strdup(arguments2[k]);
                                char pathAct[200] = "./";
                                char pathAct2[200] = "";
                                char *auxxx;
                                char *slash = "/";
                                if (palabra[0] == 47)
                                {
                                    strcat(pathAct2, arguments2[k]);
                                    strcat(pathAct2, slash);
                                    auxxx = strdup(pathAct2);
                                }
                                else
                                {
                                    strcat(pathAct, arguments2[k]);
                                    strcat(pathAct, slash);
                                    auxxx = strdup(pathAct);
                                }
                                mypath[k - 1] = strdup(auxxx);
                                mypath[k] = NULL;
                            }
                            break;
                        };
                    };
                    int a = 1;
                    //Recorremos la ruta que nos mandaron
                    for (int j = 0; mypath[j] != NULL; j++)
                    {
                        char *path = strdup(mypath[j]);
                        strcat(path, arguments2[0]);
                        if (access(path, F_OK) == 0)
                        {
                            a = 0;
                            int rc = fork();
                            forks[i] = rc;
                            forks[i + 1] = 777;
                            if (rc == 0)
                            {
                                if (isRedirection == 1)
                                {

                                    close(STDOUT_FILENO);
                                    open(file2, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
                                    execvp(path, arguments2);
                                    exit(0);
                                }

                                else
                                {

                                    execv(path, arguments2);
                                    exit(0);
                                }
                            }
                            //wait(NULL);
                            break;
                        }
                        //Validamos si el comando existe en la ruta
                    };
                    if (a == 1)
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }
                };
            };
            int status;
            for (int f = 0; forks[f] != 777; f++)
            {
                waitpid(forks[f], &status, 0);
            }
        };
        fclose(fp);
        fclose(fp2);
        exit(0);
    };

    //Mando algo erroneo
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
}