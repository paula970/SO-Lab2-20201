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

    //MODO INTERACTIVO----------------------------------------------------
    if (argc == 1)
    {

        while (1)
        {
            //Creamos un arreglo para guardar los PID de los hilos hijos
            int forks[100];
            forks[0] = 777;

            //Loop del prompt
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

            //Creamos una copia de line para validar
            char *extra = strdup(line);

            //Creamos variables auxiliares para validar
            char *palabraAux;
            char *palabraAux2;
            char *palabraAux3;

            //Bandera para validar si se ingreso almenos un comando
            int existeComando = 0;

            //Separamos por espacios, tabulación y &
            for (int j = 0; (palabraAux = strsep(&extra, " ")) != NULL; j++)
            {
                for (int k = 0; (palabraAux2 = strsep(&palabraAux, "\t")) != NULL; k++)
                {
                    for (int k = 0; (palabraAux3 = strsep(&palabraAux2, "&")) != NULL; k++)
                    {
                        if (strcmp(palabraAux3, "") != 0)
                        {
                            existeComando = 1;
                        }
                    }
                }
            }
            //Si no hay comand esperamos al siguiente comando
            if (existeComando == 0)
            {
                continue;
            }

            //Guardar cada comando en una matriz
            char *word;
            char *arguments[100];
            for (int i = 0; (word = strsep(&line, "&")) != NULL; i++)
            {
                //Guardamos los comandos exeptuando el vacio y la tabulación
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
                //Creamos variables para apuntar y validar
                char *word2;
                char *word3;
                char *arguments2[100];
                char *file = NULL;
                char *file2 = NULL;
                int isRedirection = 0;
                int error = 0;

                //Creamos una copia del comando en el que estamos
                char *args = strdup(arguments[i]);

                //Validamos si hay que redireccionar la salida a un archivo con el operador ">"
                for (int j = 0; (word2 = strsep(&args, ">")) != NULL; j++)
                {
                    if (j == 0)
                    {
                        //Validamos que no este vacio
                        if (strcmp(word2, "") != 0)
                        {
                            arguments[i] = word2;
                        }
                        else
                        {
                            error = 1;
                        }
                    }

                    //Guardamos el nombre del archivo de salida
                    else if (j == 1)
                    {
                        isRedirection = 1;
                        file = word2;
                        if (file == NULL)
                        {
                            error = 1;
                        }
                    }

                    //Si manda mas de un archivo de salida genera error
                    else
                    {
                        error = 1;
                        break;
                    }
                }

                //Si hay redirección, validamos el archivo de salida
                if (isRedirection == 1)
                {
                    //Si el archivo no es nulo
                    if (file != NULL)
                    {
                        //Creamos variables para validación
                        int soloUnArchivo = 0;
                        char *word6;
                        char *word7;

                        //Separamos el nombre del archivo por espacios y tabulaciones
                        for (int j = 0; (word6 = strsep(&file, " ")) != NULL; j++)
                        {
                            for (int k = 0; (word7 = strsep(&word6, "\t")) != NULL; k++)
                            {
                                //Validamos que no sea vacio
                                if (strcmp(word7, "") != 0)
                                {
                                    //Si no existe el archivo, se guarda el nombre
                                    if (soloUnArchivo == 0)
                                    {

                                        soloUnArchivo = 1;
                                        file2 = word7;
                                        j = j + 1;
                                    }
                                    //Si ya existia el archivo hay error
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

                //Validamossi hay error o si hay redireccion y se valida el archivo
                if (error == 1 || (file2 == NULL && isRedirection == 1))
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    break;
                }

                //Quito los espacios del comando
                for (int j = 0; (word2 = strsep(&arguments[i], " ")) != NULL; j++)
                {
                    //Quito las tabulaciones del comando
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
                        //Se valida cuando no mandan ningun path
                        if (arguments2[1] == NULL)
                        {
                            mypath[0] = NULL;
                        }

                        //liberamos la memoria de mypath y volvemos a reservar
                        free(mypath);
                        mypath = (char **)malloc(sizeof(char *));

                        //Hacemos un ciclo para guardar la dirección del path recibido
                        for (int k = 1; arguments2[k] != NULL; k++)
                        {
                            //Creamos una copia de el path recibido
                            char *letra = strdup(arguments2[k]);

                            //Creamos variables para cuando manden test o manden /bin
                            char pathAct[200] = "./";
                            char pathAct2[200] = "";
                            char *auxxx;
                            char *slash = "/";

                            //Validamos si la primera letra es un "/", por lo que seria un /bin y no se concatena nada
                            if (letra[0] == 47)
                            {
                                strcat(pathAct2, arguments2[k]);
                                strcat(pathAct2, slash);
                                auxxx = strdup(pathAct2);
                            }
                            //Si no es un "/", es un test, por lo que se debe concatenar un "./"
                            else
                            {
                                strcat(pathAct, arguments2[k]);
                                strcat(pathAct, slash);
                                auxxx = strdup(pathAct);
                            }
                            //Actualizamos mypath con el nuevo path
                            mypath[k - 1] = strdup(auxxx);
                            mypath[k] = NULL;
                        }
                        break;
                    };
                };
                //Bandera para validar que se ejetute el comando
                int a = 1;
                //Recorremos la ruta que nos mandaron
                for (int j = 0; mypath[j] != NULL; j++)
                {
                    //Creamos una copia de mypath
                    char *path = strdup(mypath[j]);

                    //Concatenamos el path con el comando
                    strcat(path, arguments2[0]);

                    //Validamos que exista el comando en esa ruta
                    if (access(path, F_OK) == 0)
                    {
                        //Se crea el nuevo hilo para ejecutar el comando
                        a = 0;
                        int rc = fork();
                        //Guardamos el PID del hilo en el arreglo
                        forks[i] = rc;
                        forks[i + 1] = 777;
                        //Si es 0 estamos en el hilo hijo
                        if (rc == 0)
                        {
                            //Validamos si hay que redireccionar la salida
                            if (isRedirection == 1)
                            {
                                //Ejecutamos el comando redireccionando a un archivo salida
                                close(STDOUT_FILENO);
                                open(file2, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
                                execvp(path, arguments2);
                                exit(0);
                            }
                            else
                            {
                                //Ejecutamos el comando normalmente
                                execv(path, arguments2);
                                exit(0);
                            }
                        }
                        break;
                    }
                };
                //En caso que no se haya ejecutado el comando hay error
                if (a == 1)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            };

            //Esperamos que terminen todos los hijos en forks
            int status;
            for (int f = 0; forks[f] != 777; f++)
            {
                waitpid(forks[f], &status, 0);
            }
        };
    };

    //MODO BATCH---------------------------------------------------------
    if (argc == 2)
    {

        //Leemos el archivo
        FILE *fp = fopen(argv[1], "r");
        //Verificamos que el archivo no sea nulo
        if (fp == NULL)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        };
        //Creamos una copia del archivo
        FILE *fp2 = fopen(argv[1], "r");

        //Creamos variables para leer por linea
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        //Leemos por linea del archivo
        while ((read = getline(&line, &len, fp)) != -1)
        {
            //Creamos un arreglo para guardar los PID de los hilos hijos
            int forks[100];
            forks[0] = 777;

            //Validamos que la linea no sea vacia
            if (strcmp(line, "") != 0)
            {
                //Eliminar espacio al final
                line[strlen(line) - 1] = '\0';

                //Creamos una copia de line para validar
                char *extra = strdup(line);

                //Creamos variables auxiliares para validar
                char *palabraAux;
                char *palabraAux2;
                char *palabraAux3;

                //Bandera para validar si se ingreso almenos un comando
                int existeComando = 0;

                //Separamos por espacios, tabulación y &
                for (int j = 0; (palabraAux = strsep(&extra, " ")) != NULL; j++)
                {
                    for (int k = 0; (palabraAux2 = strsep(&palabraAux, "\t")) != NULL; k++)
                    {
                        for (int k = 0; (palabraAux3 = strsep(&palabraAux2, "&")) != NULL; k++)
                        {
                            if (strcmp(palabraAux3, "") != 0)
                            {
                                existeComando = 1;
                            }
                        }
                    }
                }
                //Si no hay comand esperamos al siguiente comando
                if (existeComando == 0)
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
                        //Guardamos los comandos exeptuando el vacio y la tabulación
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

                    //Creamos variables para apuntar y validar
                    char *word2;
                    char *word3;
                    char *arguments2[100];
                    char *file = NULL;
                    char *file2 = NULL;
                    int isRedirection = 0;
                    int error = 0;

                    //Creamos una copia del comando en el que estamos
                    char *args = strdup(arguments[i]);

                    //Validamos si hay que redireccionar la salida a un archivo con el operador ">"
                    for (int j = 0; (word2 = strsep(&args, ">")) != NULL; j++)
                    {
                        if (j == 0)
                        {
                            //Validamos que no este vacio
                            if (strcmp(word2, "") != 0)
                            {
                                arguments[i] = word2;
                            }
                            else
                            {
                                error = 1;
                            }
                        }
                        //Guardamos el nombre del archivo de salida
                        else if (j == 1)
                        {
                            isRedirection = 1;
                            file = word2;
                            if (file == NULL)
                            {
                                error = 1;
                            }
                        }
                        //Si manda mas de un archivo de salida genera error
                        else
                        {
                            error = 1;
                            break;
                        }
                    }
                    //Si hay redirección, validamos el archivo de salida
                    if (isRedirection == 1)
                    {
                        //Si el archivo no es nulo
                        if (file != NULL)
                        {
                            //Creamos variables para validación
                            int soloUnArchivo = 0;
                            char *word6;
                            char *word7;

                            //Separamos el nombre del archivo por espacios y tabulaciones
                            for (int j = 0; (word6 = strsep(&file, " ")) != NULL; j++)
                            {
                                for (int k = 0; (word7 = strsep(&word6, "\t")) != NULL; k++)
                                {
                                    //Validamos que no sea vacio
                                    if (strcmp(word7, "") != 0)
                                    {
                                        //Si no existe el archivo, se guarda el nombre
                                        if (soloUnArchivo == 0)
                                        {

                                            soloUnArchivo = 1;
                                            file2 = word7;
                                            j = j + 1;
                                        }
                                        //Si ya existia el archivo hay error
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

                    //Validamossi hay error o si hay redireccion y se valida el archivo
                    if (error == 1 || (file2 == NULL && isRedirection == 1))
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        break;
                    }
                    //Quito los espacios del comando
                    for (int j = 0; (word2 = strsep(&arguments[i], " ")) != NULL; j++)
                    {
                        //Quito las tabulaciones del comando
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
                            //Se valida cuando no mandan ningun path
                            if (arguments2[1] == NULL)
                            {
                                mypath[0] = NULL;
                            }

                            //liberamos la memoria de mypath y volvemos a reservar
                            free(mypath);
                            mypath = (char **)malloc(sizeof(char *));

                            //Hacemos un ciclo para guardar la dirección del path recibido
                            for (int k = 1; arguments2[k] != NULL; k++)
                            {
                                //Creamos una copia de el path recibido
                                char *letra = strdup(arguments2[k]);

                                //Creamos variables para cuando manden test o manden /bin
                                char pathAct[200] = "./";
                                char pathAct2[200] = "";
                                char *auxxx;
                                char *slash = "/";

                                //Validamos si la primera letra es un "/", por lo que seria un /bin y no se concatena nada
                                if (letra[0] == 47)
                                {
                                    strcat(pathAct2, arguments2[k]);
                                    strcat(pathAct2, slash);
                                    auxxx = strdup(pathAct2);
                                }
                                //Si no es un "/", es un test, por lo que se debe concatenar un "./"
                                else
                                {
                                    strcat(pathAct, arguments2[k]);
                                    strcat(pathAct, slash);
                                    auxxx = strdup(pathAct);
                                }
                                //Actualizamos mypath con el nuevo path
                                mypath[k - 1] = strdup(auxxx);
                                mypath[k] = NULL;
                            }
                            break;
                        };
                    };
                    //Bandera para validar que se ejetute el comando
                    int a = 1;
                    //Recorremos la ruta que nos mandaron
                    for (int j = 0; mypath[j] != NULL; j++)
                    {
                        //Creamos una copia de mypath
                        char *path = strdup(mypath[j]);

                        //Concatenamos el path con el comando
                        strcat(path, arguments2[0]);

                        //Validamos que exista el comando en esa ruta
                        if (access(path, F_OK) == 0)
                        {
                            //Se crea el nuevo hilo para ejecutar el comando
                            a = 0;
                            int rc = fork();
                            //Guardamos el PID del hilo en el arreglo
                            forks[i] = rc;
                            forks[i + 1] = 777;
                            //Si es 0 estamos en el hilo hijo
                            if (rc == 0)
                            {
                                //Validamos si hay que redireccionar la salida
                                if (isRedirection == 1)
                                {
                                    //Ejecutamos el comando redireccionando a un archivo salida
                                    close(STDOUT_FILENO);
                                    open(file2, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
                                    execvp(path, arguments2);
                                    exit(0);
                                }

                                else
                                {
                                    //Ejecutamos el comando normalmente
                                    execv(path, arguments2);
                                    exit(0);
                                }
                            }
                            break;
                        }
                    };
                    //En caso que no se haya ejecutado el comando hay error
                    if (a == 1)
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }
                };
            };
            //Esperamos que terminen todos los hijos en forks
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

    //Mensaje de error
    write(STDERR_FILENO, error_message, strlen(error_message));
    //Fin del programa
    exit(1);
}