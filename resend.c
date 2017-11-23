
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define INPUT_BUFFER_SIZE 2048

/* Copie tout ce qui arrive dans le descripteur de fichier fd_in dans le
 * descripteur de fichier fd_out */
void forward(int fd_in, int fd_out) {
	char buffer[INPUT_BUFFER_SIZE];

	for(;;) {
		size_t nb_read; /* Nombre d'octets lu sur l'entrée */

		/* Lecture sur le premier descripteur de fichier */
		nb_read=read(fd_in, buffer, INPUT_BUFFER_SIZE);
		if (nb_read==0) {
			/* Il n'y a plus rien à lire : on quite la boucle */
			break;
		}
		if (nb_read==-1) {
			/* Il y a eu une erreur */
			if (errno != EINTR) {
				/* On abandonne si ce n'était pas un signal */
				perror("Read error");
				exit(1);
			}
			/* C'est juste un signal qui a interrompu le read,
			 * on reprend au début de la boucle */
			continue;
		}

		size_t nb_write; /* nombre d'octets écrits au total sur la
				  * sortie */
		size_t nb; /* nombre d'octets écrits sur la sortie
			    * par le dernier write */
		/* Note: la plupart du temps, le système va écrire toutes les
		 * données en une fois et cette boucle for est inutile (elle
		 * n'est parcourue qu'une seule fois) */
		for(nb_write=0; nb_write<nb_read; nb_write+=nb) {
			/* écriture sur la sortie */
			nb=write(fd_out, buffer+nb_write, nb_read-nb_write);
			if (nb==-1 && errno != EINTR) {
				/* Il y a eu une erreur et ce n'était pas un
				 * signal: on abandonne */
				perror("Write error");
				exit(1);
			}
		}

	}
}


int main() {
	printf("Début de la copie\n");
	/* On copie tout ce qui arrive sur l'entrée standard (0) sur la sortie
	 * standard (1) */
	forward(0,1);
	printf("Fin de la copie\n");
	return 0;
}
