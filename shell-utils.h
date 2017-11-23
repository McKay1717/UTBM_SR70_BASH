
#ifndef SHELL_UTILS
#define SHELL_UTILS

/* découpe la chaine 'cmd' en multiples chaines stockées dans le tableau
 * 'tokens'. La séparation se fait sur les blancs (' \t\n')
 * PARAMETRES :
 * * tokens : le tableau de retour
 * * cmd : la chaine à séparer (elle sera modifiée)
 * * max_token : le nombre maximum de token que l'on peut extraire dans
 *   tokens
 * VALEUR DE RETOUR :
 * Le nombre de tokens découpés et stockés dans 'tokens' ou -1 s'il y a eu
 * une erreur.
 * PARTICULARITE :
 * Une valeur NULL est mise à la fin du tableau tokens (ie prévoir une
 * place de plus)
 * EXEMPLES :
 *   char* tokens[5];
 *   int nbtok;
 *   nbtok=split_tokens(tokens, "ls -l", 4);
 *   // => nbtok=2
 *   // => tokens={"ls", "-l", NULL}
 *   nbtok=split_tokens(tokens, "ls -l a b c", 4);
 *   // => nbtok=4
 *   // => tokens={"ls", "-l", "a", "b", NULL}
 */
int split_tokens(char **tokens, char* cmd, int max_token);


/* recherche une redirection dans un tableau de tokens
 * PARAMETRES :
 * * tokens : le tableau de tokens. Si une redirection est trouvée, elle
 *   est supprimée de ce tableau.
 * * redirection : la chaine introduisant la redirection (généralement ">"
 *   ou "<")
 * VALEUR DE RETOUR :
 * La chaine suivant celle de redirection dans les tokens (ie le fichier
 * redirigé). NULL si on ne trouve pas de redirection.
 * EXEMPLES :
 *   char* tokens[5]={"ls", "-l", ">", "toto", "shell.c", NULL}
 *   char *file_in, *file_out;
 *   file_in=trouve_redirection(tokens, "<");
 *   // => file_in=NULL
 *   // => tokens={"ls", "-l", ">", "toto", "shell.c", NULL}
 *   file_out=trouve_redirection(tokens, ">");
 *   // => file_out="toto"
 *   // => tokens={"ls", "-l", "shell.c", NULL}
 */
char* trouve_redirection(char **tokens, const char* redirection);

/* recherche le premier tube dans un tableau de tokens et coupe ce tableau
 * en deux.
 * PARAMETRES :
 * * tokens : le tableau de tokens. Si un tube est trouvé, le tableau de
 *   tokens est modifié pour s'arreter à la fin de la première commande
 *   (ie avant le tube)
 * * tube : la chaine introduisant le tube (a priori "|")
 * VALEUR DE RETOUR :
 * Le tableau de tokens de la commande après le tube ou NULL si on ne trouve
 * pas de tube.
 * EXEMPLES :
 *   char* tokens[5]={"ls", "-l", "|", "wc", "-l", NULL}
 *   char **tokens_cmd2, **tokens_cmd3;
 *   tokens_cmd2=trouve_tube(tokens, "|");
 *   // => tokens_cmd2={"wc", "-l", NULL}
 *   // => tokens={"ls", "-l", NULL}
 *   tokens_cmd3=trouve_tube(tokens_cmd2, "|");
 *   // => tokens_cmd3=NULL
 *   // => tokens_cmd2={"wc", "-l", NULL}
 */
char** trouve_tube(char **tokens, const char* tube);

#endif
