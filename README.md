# ProgSystTP_ENSEA

**Q1 -**

Pour afficher un message d'accueil et un prompt simple, on définit deux chaînes de caractères `welcome_msg` et `prompt`. 

Pour les afficher sans utiliser `printf`, on utilise la fonction `write()`, définie dans `<unistd.h>`. Cette fonction effectue une demande directe au noyau, c'est un appel système (syscall). 

Le premier paramètre de cette fonction correspond au file descriptor. Les 3 descripteurs par défaut sont :
- `STDIN_FILENO` : 0 pour entrée standard
- `STDOUT_FILENO` : 1 pour sortie standard
- `STDERR_FILENO` : 2 pour erreur standard

En utilisant le descripteur `STDOUT_FILENO`, on écrit directement dans le terminal. La fonction écrit des octets dont le nombre est défini par le troisième paramètre. 


**Q2 -**

On écrit la ligne d'affichage du prompt à l'intérieur du while afin de permettre le retour au prompt après l'exécution d'une commande. 

La lecture de la commande saisie est assurée par la fonction `read(FD, buffer, count)` définie dans `<unistd.h>`. Comme la fonction `write()`, elle prend en paramètre un file descriptor (ici 0 pour l'entrée standard correspondand au clavier). Le noyau copie jusqu'à `count` octets dans `buffer`. 

Le buffer est un tableau qui contient tous les caractères entrés par l'utilisateur. Le dernier caractère est `'\n'` qui correspond à Enter. \
On remplace donc `'\n'` par le null terminator `'\0'`, de manière à récupérer uniquement la commande. 

Afin d'exécuter les fonctions voulues sans utliliser `system`, on utilise à la place `execlp`, fonction définie dans `<unistd.h>`. Cette fonction permet de remplacer le programme courant par un autre. 

La fonction va : 
- chercher un programme dans le PATH
- charger ce programme
- remplacer le processus courant par ce programme

Après l'exécution, le code situé après `exec()` ne s'exécute pas. 

_Problème rencontré :_ On n'avait pas téléchargé `fortune` au départ, ce qui fait que l'exécution ne fonctionnait pas. 

**Q3 -**

Pour assurer la gestion de la sortie du shell avec la commande `"exit"` ou un `<ctrl>+D`, on modifie la fonction `exeCommand`. 

On teste donc si la commande entrée par l'utilisateur correspond à "exit" ou si aucun caractère n'a été transmis. \
En effet, la commande `<ctrl>+D`, qui renvoie le message correspondant à End Of File, est interprétée comme s'il n'y a plus rien à lire sur l'entrée standard. 

On a ajouté l'exécution de la commande de sortie du shell avant `fork()` pour éviter que d'autres processus soient créés ou exécutés avant le "exit". 


**Q4 -**

Il s'agit maintenant d'afficher le code de retour (ou du signal) de la commande précédente dans le prompt. 

La ligne qui permet d'afficher le prompt `enseash %` a été déplacée en dehors du while de manière à ce qu'il n'apparaisse qu'une seule fois au début. 

Le fils doit exécuter la commande demandée et ne jamais revenir dans le shell car l'exécution remplace le processus courant. Le père gère le shell : il doit attendre la fin du fils, récupérer le code retour ou le signal et afficher les informations de fin.

La fonction `writeReturn` permet de mettre en forme la chaîne de caractères contenant le code de sortie. 

Pour vérifier le bon fonctionnement de l'affichage du code de retour, on teste sur un autre programme `'prog_test'` dont on fixe le code de sortie. En appelant le programme dans le shell `enseash`, le code de sortie fixé s'affiche correctement dans le prompt suivant.

On peut également tester pour un signal. Pour cela, on modifie le code dans le fils. \
On commence par faire afficher dans le terminal le pid du process grâce à `getpid()`. On ajoute un temps d'attente afin de pouvoir arrêter le processus avant la fin. Depuis un autre terminal, on utilise `kill -SIGKILL xxx` pour arrêter le processus fils. Dand le shell `enseash`, on observe l'affichage du code `sign:9` correspondant bien à SIGKILL. 

_Note :_ Il est conseillé de ne pas utiliser `printf` car il peut mal fonctionner avec `read` et `write`, mais par facilité pour les tests, on fait afficher le pid avec cette fonction. 
