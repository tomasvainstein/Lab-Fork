#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define ERROR -1

void
chequear_error(int i, char *mensaje_error)
{
	if (i < 0) {
		perror(mensaje_error);
		exit(ERROR);
	}
}

void
primera_iteracion(int inicio, int fin, int fd_padre)
{
	for (int i = inicio; i <= fin; i++)
		chequear_error(write(fd_padre, &i, sizeof(int)),
		               "Error al escrbir");

	close(fd_padre);
}

void
clasificar(int fd_izq)
{
	int primo;
	int read_aux = read(fd_izq, &primo, sizeof primo);

	if (read_aux == 0) {
		close(fd_izq);
		return;
	}

	printf("primo %i\n", primo);

	int fds_der[2];
	chequear_error(pipe(fds_der), "error de pipe");

	pid_t pid_hijo = fork();
	chequear_error(pid_hijo, "Error de fork");

	if (pid_hijo == 0) {
		close(fds_der[WRITE]);
		close(fd_izq);
		clasificar(fds_der[READ]);
		close(fd_izq);
	} else {
		close(fds_der[READ]);
		int numero;
		read_aux = read(fd_izq, &numero, sizeof numero);

		while (read_aux > 0) {
			if (numero % primo != 0) {
				chequear_error(write(fds_der[WRITE],
				                     &numero,
				                     sizeof numero),
				               "fallo de escritura");
			}
			read_aux = read(fd_izq, &numero, sizeof numero);
		}

		close(fds_der[WRITE]);
		close(fd_izq);
		wait(NULL);
	}
}

int
main(int argc, char *argv[])
{
	if (argc < 2)
		exit(ERROR);

	int num = atoi(argv[1]);

	int fd_padre[2];
	chequear_error(pipe(fd_padre), "Error de pipe");

	pid_t pid_hijo = fork();
	chequear_error(pid_hijo, "Error haciendo fork");

	if (pid_hijo == 0) {
		close(fd_padre[WRITE]);
		clasificar(fd_padre[READ]);
		close(fd_padre[READ]);
	} else {
		close(fd_padre[READ]);
		primera_iteracion(2, num, fd_padre[WRITE]);
		close(fd_padre[WRITE]);
		wait(NULL);
	}

	return 0;
}
