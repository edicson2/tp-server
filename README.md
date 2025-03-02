# Algorithme du banquier | DeadLocks

Implémentation en C l’algorithme du banquier, un algorithme qui permet de gérer l’allocation des différents types des ressources, tout en évitant les interblocages (deadlocks). Le code fourni implémente un modèle de client-serveur qui utilise les prises (sockets) comme moyen de communication.

D’un côté l’application serveur est analogue à un système d’exploitation qui gère l’allocation des ressources, comme par exemple la mémoire, le disque dur ou les imprimantes. Ce serveur reçoit simultanément plusieurs demandes de ressources des différents clients à travers des connexions. Pour chaque connexion/requête, le serveur doit décider quand les ressources peuvent être allouées au client, de façon à éviter les interblocages en suivant l’algorithme du banquier.

De l’autre côte, l’application client simule l’activité de plusieurs clients dans différents fils d’exécution (threads). Ces clients peuvent demander des ressources si elles sont disponibles ou libérer des ressource qu’ils détiennent à ce moment.

On va mettre en pratique quatre sujets différents:
• Fils d’exécution multiples (multithreading).
• Prévention des séquencements critiques (race conditions).
• Évitement d’interblocages (deadlock avoidance).
• Communication entre processus via des prises (sockets).

## Server
<p align="center"> <img src="/Demo-server.gif?raw=true"/> </p>

## Client
<p align="center"> <img src="/Demo-client.gif?raw=true"/></p>


## Compiling

Compiler les deux applications.

```bash
make all 
```

Lancer les client et le serveur.

```bash
make run 
```

Lancer le serveur.

```bash
make run-server
```

Lancer le client.

```bash
make run-client
```

Nettoyer le dossier build.

```bash
make clean
```

