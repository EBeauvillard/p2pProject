#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "processing.h"
#include "cmd.h"

#define SIZE_REQUEST 2048

void thread_fct(void * sockfd) {
  if(verbose) {
    printf("New thread created.\n");
    printf("Waiting connection...\n");
  }
  struct sockaddr_in cli_addr;
  unsigned int clilen = sizeof(cli_addr);
  int client_socket = accept(*(int*)sockfd,
              (struct sockaddr *) &cli_addr,
              &clilen);
  if(client_socket < 0) {
    fprintf(stderr, "Error on accept.\n");
    exit(0);
  }
  if(verbose) {
    printf("Connection established.\n");
  }
  //nouveau thread pour le prochain client
  pthread_t next_thread;
  pthread_create(&next_thread, NULL, (void * (*) (void *))thread_fct, sockfd);
  char response[SIZE_REQUEST];
  int comp = 0;
  char * request_tot;
  while (1) {
    if(comp>0) {
      if(verbose) {
        printf("Requête longue ou séparation en fragments TCP. Récupération et concatenation des segments ...\n");
      }
      char request[comp*SIZE_REQUEST];
      strcpy(request, request_tot);
      if(verbose) {
        printf("Nouvel espace mémoire réservé de taille %ld\n", comp*sizeof(char)*SIZE_REQUEST);
      }
      request_tot=realloc(request_tot, (comp+1)*SIZE_REQUEST*sizeof(char));
      bzero(request_tot, (comp+1)*SIZE_REQUEST);
      strcpy(request_tot, request);
    } else {
      request_tot = malloc(SIZE_REQUEST*sizeof(char));
      bzero(request_tot, SIZE_REQUEST);
    }
    int n = read(client_socket, request_tot + comp*SIZE_REQUEST, SIZE_REQUEST);
    comp++;
    if (n < 0) {
      fprintf(stderr, "Error reading from socket.\n");
      exit(0);
    }
    if (n == 0) {
      struct user * uti_to_supr;
      if(!find_user(cli_addr.sin_port, inet_ntoa(cli_addr.sin_addr), &uti_to_supr)) {
        exit(0);
      }
      delete_user(uti_to_supr);
      //client deco
      if(verbose) {
        printf("Peer disconnected.\n");
      }
      close(client_socket);
      break;
    }
    if(request_tot[n-1+(comp-1)*SIZE_REQUEST] == '\n') {
      comp=0;
    } else {
      continue; //on retourne lire la suite de la requête
    }

    if(verbose) {
      printf("Request received : \n  \"%s\"\n", request_tot);
    }
    bzero(response, SIZE_REQUEST);
    char ip_adress[32];
    strcpy(ip_adress, inet_ntoa(cli_addr.sin_addr));
    int ret = request_identification(request_tot, response, ip_adress, cli_addr.sin_port, verbose);
    if(ret) {
      if(verbose) {
        printf("Sending error to client socket %d ...\n", client_socket);
        printf("Waiting another request ...\n");
      }
      n = write(client_socket, "Unexpected error\n", 17);
    } else {
      if(verbose) {
        printf("Sending response to client socket %d ...\n", client_socket);
        printf("Waiting another request ...\n");
      }
      n = write(client_socket,response, strlen(response));
      if (n < 0) {
        fprintf(stderr, "Error writing to socket.\n");
      }
    }
    free(request_tot);
  }
  pthread_join(next_thread, NULL);
  pthread_exit(NULL);
}

void get_ip(int fd, char* ip_address) {
  char buffer[SIZE_BUFFER + 1];
  read(fd, buffer, SIZE_BUFFER);
  char* word_server = "server=";
  int len_server = strlen(word_server);
  char word[len_server + 1];
  int i = 0;
  int c = 0;

  while ((buffer[c] != EOF) || (c == SIZE_BUFFER)) {
    word[i] = buffer[c];
    i++;
    if ((buffer[c] == '\n') || (i == len_server)) {
      word[i] = 0;
      if (!strcmp(word, word_server)) {
        i = 0;
        c++;
        while ((buffer[c] != '\n') && (i < SIZE_IP - 1)) {
          ip_address[i] = buffer[c];
          i++;
          c++;
        }
        ip_address[i] = 0;
        return;
      }
      i = 0;
    }
    c++;
  }
  if (buffer[c] == EOF) {
    fprintf(stderr, "Error, ip address not found.\n");
  } else {
    fprintf(stderr, "Error, file config.ini is too big.\n");
  }
}

void create_config_ini(int port_server) {
  system("echo \"[database]\" > config.ini");
  system("echo -n \"server=\" >> config.ini");
  system("ip route get 1.2.3.4 | awk '{print $7}' | head -n -1 >> config.ini");
  int fd = open("config.ini", O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
  char buffer[10];
  char* params_name[5] = { "portclient", "portserver", "piecesize", "messagesize", "nb_clients_max" };
  int params[5] = { port_client, port_server, piece_size, message_size, nb_clients_max };
  for (int i = 0 ; i < 5 ; i++) {
    char command[100] = "echo -n \"";
    char* redirect = "=\" >> config.ini";
    strcat(command, params_name[i]);
    strcat(command, redirect);
    system(command);
    sprintf(buffer, "%d", params[i]);
    write(fd, buffer, strlen(buffer));
    system("echo  \"\" >> config.ini");
  }
}

int main(int argc, char* argv[]) {
  //server
  int sockfd;
  struct sockaddr_in serv_addr;
  if (argc < 2) {
      fprintf(stderr,"Error, no port provided\n");
      exit(0);
  }

  int ind = parse_opts(argc, argv);

  int port_server = atoi(argv[ind]);
  create_config_ini(port_server);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
     fprintf(stderr, "Error opening socket.\n");
     exit(0);
   }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  char ip_address[SIZE_IP];
  int fd = open("config.ini", S_IRUSR);
  if (fd == -1) {
    fprintf(stderr, "Error : file config.ini not found.\n");
    exit(0);
  }

  get_ip(fd, ip_address);

  inet_aton(ip_address, &serv_addr.sin_addr);
  serv_addr.sin_port = htons(port_server);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error on binding.\n");
    exit(1);
  }
  listen(sockfd,5);
  printf("Server address : %s !\n", inet_ntoa(serv_addr.sin_addr));
  //thread creat
  pthread_t first_thread;
  pthread_create(&first_thread, NULL, (void * (*) (void *))thread_fct, (void *)&sockfd);
  pthread_join(first_thread, NULL);
  return EXIT_SUCCESS;
}
