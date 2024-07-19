#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NARGS
#define NARGS 4
#endif

#define ERROR -1

void
ejecutar(char *args[], int n_lineas, char *comando)
{
	args[n_lineas + 1] = NULL;
	int estado;

	int fork_aux = fork();

	if (fork_aux < 0) {
		perror("fallo fork");
		exit(ERROR);
	}

	if (fork_aux > 0) {
		wait(&estado);
		for (int i = 1; i < NARGS + 2; i++) {
			free(args[i]);
			args[i] = NULL;
		}
	}

	if (fork_aux == 0) {
		execvp(comando, args);
		perror("fallo execvp");
		for (int i = 1; i < NARGS + 2; i++) {
			free(args[i]);
			args[i] = NULL;
		}
		exit(ERROR);
	}
}

void
procesar_comando(char *comando)
{
	size_t tamanio_buffer = 0;
	int leido;
	char *args[NARGS + 2] = { comando, NULL };
	int n_lineas = 0;

	while ((leido = getline(&args[n_lineas + 1], &tamanio_buffer, stdin)) !=
	       -1) {
		int longitud = strlen(args[n_lineas + 1]);
		if (args[n_lineas + 1][longitud - 1] == '\n')
			args[n_lineas + 1][longitud - 1] = '\0';

		n_lineas++;

		if (n_lineas == NARGS) {
			ejecutar(args, n_lineas, comando);
			n_lineas = 0;
		}
	}

	if (n_lineas > 0)
		ejecutar(args, n_lineas, comando);
}

int
main(int argc, char *argv[])
{
	if (argc < 2)
		exit(ERROR);

	procesar_comando(argv[1]);

	return 0;
}
