//#define _XOPEN_SOURCE 700   /* So as to allow use of `fdopen` and `getline`.  */

#include "common.h"
#include "server_thread.h"

#include <netinet/in.h>
#include <netdb.h>

#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>

#include <time.h>

enum { NUL = '\0' };

enum {
    /* Configuration constants.  */
            max_wait_time = 30,
    server_backlog_size = 5
};

unsigned int server_socket_fd;

// Nombre de client enregistré.
int nb_registered_clients;

// Variable du journal.
// Nombre de requêtes acceptées immédiatement (ACK envoyé en réponse à REQ).
unsigned int count_accepted = 0;

// Nombre de requêtes acceptées après un délai (ACK après REQ, mais retardé).
unsigned int count_wait = 0;

// Nombre de requête erronées (ERR envoyé en réponse à REQ).
unsigned int count_invalid = 0;

// Nombre de clients qui se sont terminés correctement
// (ACK envoyé en réponse à CLO).
unsigned int count_dispatched = 0;

// Nombre total de requête (REQ) traités.
unsigned int request_processed = 0;

// Nombre de clients ayant envoyé le message CLO.
unsigned int clients_ended = 0;

// TODO: Ajouter vos structures de données partagées, ici.
int *available;
int *allocation;
int *max;
int *need;
int *ressource;


/********************************************************************************************************************/

void recevoir_ressource(int socket_fd){
  int x=0;
  int ress[2]={4};
  struct cmd_header_t header = { .nb_args=0 };

  int le = read_socket(socket_fd, &header, sizeof(header),max_wait_time * 1000);

  //ici on va faire une boucle pour initialiser les elements pour l'algo
  // du banquier;

  //printf("commande %d\n",header.cmd);


  if(header.cmd==1){
    printf("**************************Ressources*************************\n");
    printf("CONF %d",header.nb_args);
    for(int i=0; i<header.nb_args; i++){
      int le=read_socket(socket_fd,&x,sizeof(x),max_wait_time*1000);

      printf(" %d",x);
    }
    printf("\n");

    send(socket_fd ,&ress, sizeof(ress) ,0);
  }
  else if(header.cmd==8){

    printf("ERR\n");
  }
  close(socket_fd);
  //close(server_socket_fd);


}


void recevoir_beg(int socket_fd){
  int x=0;
  printf("************************Initialisation du serveur********************\n\n");

  struct cmd_header_t header = { .nb_args = 0 };

  int ack[3]={ACK,1};
  //ack[0]=4;
  int len = read_socket(socket_fd, &header, sizeof(header), max_wait_time * 1000);
  if(len>0){
    if(header.cmd==BEGIN){
      int lenn= read_socket(socket_fd, &x, sizeof(x), max_wait_time * 1000);
      printf("BEGIN %d %d\n",header.nb_args,x);
      send(socket_fd ,ack, sizeof(ack) ,0);
      // memset(&header,0,sizeof(header));
      //recevoir_ressource(socket_fd);
    }
    else if(header.cmd==8){

      printf("ERR\n");
    }
  }else{}
  close(socket_fd);
  //close(server_socket_fd);

}





void gerer_ini(int socket_fd,int cmd,int nb_args){


  int x=-11;

  if(cmd==INIT){
    //printf("************************INI********************\n\n");
    printf("INIT");
    for(int i=0; i<nb_args+1; ++i){
      int len = read_socket(socket_fd,&x,sizeof(x),max_wait_time*1000);

      printf(" %d",x);

    }
    printf("\n");
    printf("Sending reponse de INIT\n");
    //int ack[2] = {4, 0};
    //send(socket_fd ,ack, sizeof(ack) ,0);
  } else if(cmd==REQ){

    for(int i=0; i<nb_args+1; ++i){
      int le=read_socket(socket_fd,&x,sizeof(x),max_wait_time*1000);

      printf(" %d",x);

    }
    printf("\n");
    int ack[2] = {4, 0};
    printf("Sending ACK 0..\n");
    send(socket_fd ,ack, sizeof(ack) ,0);

  }
  else{
    printf("ERR\n");
    //Emvoyer erreur.
  }

}


int accepte_ct(){
  struct sockaddr_in thread_addr;
  socklen_t socket_len = sizeof (thread_addr);
  int thread_socket_fd = -1;
  while (thread_socket_fd < 0)
  {
    thread_socket_fd = accept (server_socket_fd, (struct sockaddr *) &thread_addr, &socket_len);
  }
  return thread_socket_fd;
}



/********************************************************************************************************************/


void
st_init ()
{
  // Initialise le nombre de clients connecté.
  nb_registered_clients = 0;

  // TODO

  // Attend la connection d'un client et initialise les structures pour
  // l'algorithme du banquier.

  // END TODO
  int socket=accepte_ct();

  recevoir_beg(socket);

  socket=accepte_ct();

  recevoir_ressource(socket);
//close(thread_socket_fd);
//close(socket);
}

void
st_process_requests (server_thread * st, int socket_fd)
{
  // TODO: Remplacer le contenu de cette fonction


  struct cmd_header_t header = { .nb_args = 0 };

  int len = read_socket(socket_fd, &header, sizeof(header), max_wait_time*1000);

  int i=0;

  while (i < 3) {

    if (len > 0) {

      if (len != sizeof(header.cmd) && len != sizeof(header)) {
        printf ("Thread %d received invalid command size=%d!\n", st->id, len);

      }

      printf("\nThread %d received command=%d, nb_args=%d\n", st->id, header.cmd, header.nb_args);
      //dispatch of cmd void thunk(int sockfd, struct cmd_header* header);
      //if(header.cmd==INIT){
      gerer_ini(socket_fd,header.cmd,header.nb_args);

      //}
      //else if(header.cmd==REQ){

      //gerer_ini(socket_fd,header.cmd,header.nb_args);

      //i=i+1;
      //}

    } else {
      if (len == 0) {
        fprintf(stderr, "Thread %d, connection timeout\n", st->id);
      }
      printf("i= %d\n",i);
    }
    ++i;
  }
}


void
st_signal ()
{
  // TODO: Remplacer le contenu de cette fonction



  // TODO end
}

void *
st_code (void *param)
{
  server_thread *st = (server_thread *) param;

  struct sockaddr_in thread_addr;
  socklen_t socket_len = sizeof (thread_addr);
  int thread_socket_fd = -1;
  int end_time = time (NULL) + max_wait_time;

  // Boucle jusqu'à ce que `accept` reçoive la première connection.
  while (thread_socket_fd < 0)
  {
    thread_socket_fd =
            accept (server_socket_fd, (struct sockaddr *) &thread_addr,
                    &socket_len);

    if (time (NULL) >= end_time)
    {
      break;
    }
  }

  // Boucle de traitement des requêtes.
  while (accepting_connections)
  {
    if (!nb_registered_clients && time (NULL) >= end_time)
    {
      fprintf (stderr, "Time out on thread %d.\n", st->id);
      pthread_exit (NULL);
    }
    if (thread_socket_fd > 0)
    {
      st_process_requests (st, thread_socket_fd);
      close (thread_socket_fd);
      end_time = time (NULL) + max_wait_time;
    }
    thread_socket_fd =
            accept (server_socket_fd, (struct sockaddr *) &thread_addr,
                    &socket_len);
  }
  return NULL;
}


//
// Ouvre un socket pour le serveur.
//
void
st_open_socket (int port_number)
{
  server_socket_fd = socket (AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (server_socket_fd < 0)
    perror ("ERROR opening socket");

  if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
    perror("setsockopt()");
    exit(1);
  }

  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons (port_number);

  if (bind
              (server_socket_fd, (struct sockaddr *) &serv_addr,
               sizeof (serv_addr)) < 0)
    perror ("ERROR on binding");

  listen (server_socket_fd, server_backlog_size);
}


//
// Affiche les données recueillies lors de l'exécution du
// serveur.
// La branche else ne doit PAS être modifiée.
//
void
st_print_results (FILE * fd, bool verbose)
{
  if (fd == NULL) fd = stdout;
  if (verbose)
  {
    fprintf (fd, "\n---- Résultat du serveur ----\n");
    fprintf (fd, "Requêtes acceptées: %d\n", count_accepted);
    fprintf (fd, "Requêtes : %d\n", count_wait);
    fprintf (fd, "Requêtes invalides: %d\n", count_invalid);
    fprintf (fd, "Clients : %d\n", count_dispatched);
    fprintf (fd, "Requêtes traitées: %d\n", request_processed);
  }
  else
  {
    fprintf (fd, "%d %d %d %d %d\n", count_accepted, count_wait,
             count_invalid, count_dispatched, request_processed);
  }
}