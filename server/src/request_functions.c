#include "request_functions.h"

int announce(char* request, char* ip, int port_serv, bool verbose) {
  if(verbose) {
    printf("   Processing request announce received by %s.\n", ip);
  }
  char buffer[100];
  if (!next_word(&request, buffer, verbose)) //listen
    return -1;
  if (!next_word(&request, buffer, verbose)) //$port
    return -1;
  int port_cli = atoi(buffer);

  if (!next_word(&request, buffer, verbose)) //seed
    return -1;
  if(new_user(port_serv, port_cli, ip)) { //dans le cas de déclaration de nouveau fichier,
  // cette opération ne fait rien car l'utilisateur est deja enregistré
    if(verbose) {
      printf("   New user added.\n");
    }
  }
  struct user * us;
  find_user(port_serv, ip, &us);
  if (us == NULL) {
    if(verbose) {
      printf("   ERROR : User not found in announce.\n");
    }
    return -1;
  }
  struct file * fi;
  while(strcmp(buffer, "leech") && next_word(&request, buffer, verbose)) { //$Filename
    char name[25];
    strcpy(name, buffer);
    if (!next_word(&request, buffer, verbose)) //$length
      return -1;
    int length = atoi(buffer);
    if (!next_word(&request, buffer, verbose)) //$pieceSize
      return -1;
    int piece_size = atoi(buffer);
    if (!next_word(&request, buffer, verbose))//$key
      return -1;
    new_file(name, length, piece_size, buffer);
    if(verbose) {
      printf("   New file added ...\n");
    }
    find_file(buffer, &fi);
    new_linked_file(&(us->owned_file), fi);
    new_linked_user(&(fi->owned_by), us);
  }

  if(strcmp(buffer, "leech")) {
    if(verbose) {
      printf("   End of request announce.\n");
    }
    return 0;
  } else {
    while(next_word(&request, buffer, verbose)) {
      find_file(buffer, &fi);
      new_linked_file(&(us->leeching), fi);
      new_linked_user(&(fi->leeched_by), us);
    }
    if(verbose) {
      printf("   End of request announce.\n");
    }
    return 0;
  }
}

char* get_file(char* request, bool verbose) {
  if(verbose) {
    printf("   Begin of processing request get_file.\n");
  }
  char descriptor[100];
  if (!next_word(&request, descriptor, verbose)) {
    return NULL;
  }

  file_t* fi = NULL;
  if (!find_file(descriptor, &fi)) {
    fprintf(stderr, "Error : no file found.\n");
    return NULL;
  }
  if(verbose) {
    printf("   Begining search in database ...\n");
  }
  char* adr = malloc(sizeof(char) * 1024);
  strcpy(adr, "peers ");
  strcat(adr, descriptor);
  strcat(adr, " [");
  linked_user_t* l_u = fi->owned_by;
  while (l_u != NULL) {
    char buffer[20];
    sprintf(buffer, "%s", l_u->u->IP_adr);
    strcat(adr, buffer);
    sprintf(buffer,":%d ", l_u->u->port_cli);
    strcat(adr, buffer);
    l_u = l_u->next;
  }
  int s = strlen(adr);
  if (adr[s - 1] != '[') {
    adr[s - 1] = ']';
    s = s - 1;
  } else {
    strcat(adr, "]");
  }
  adr[s+1] = '\n';
  adr[s + 2 ] = 0;
  if(verbose) {
    printf("   End of search in database.\n");
    printf("   End of request get_file.\n");
  }
  return adr;
}

char* look(char* request, bool verbose) {
  if(verbose) {
    printf("   Processing request look.\n");
  }
  char* buffer = NULL,* list = NULL;
  char** crit = NULL,** cond = NULL,** param = NULL;
  int list_length = 0;
  if(verbose) {
    printf("   Processing parameters ...\n");
  }
  // Parses all criterions given in request.
  if (malloc_look(&buffer, &crit, &cond, &param, &list) == -1)
    return NULL;
  strcpy(list, "list [");
  list_length=strlen(list);
  int nb_criterion = 0;
  while (next_word(&request, buffer, verbose)) {
    if (malloc_criterion(&crit[nb_criterion], &cond[nb_criterion],
        &param[nb_criterion]) == -1)
        return NULL;

    if (split_criterion(crit[nb_criterion], cond[nb_criterion],
        param[nb_criterion], buffer)) {
      fprintf(stderr, "Wrong syntax for criterion.\n");
      fprintf(stderr, "Usage : < look [filename=\"file_a.dat\" filesize>\"1048576\"]\n");
      free_look(buffer, crit, cond, param, list, nb_criterion);
      return NULL;
    }
    nb_criterion++;
  }
  if(verbose) {
    printf("   Options processed.\n");
    printf("   Beginning of search ...\n");
  }

  // Checks all files contained in the server and selects those which
  // hold all criterions.
  file_t * file = server_file_memory;
  while (file != NULL) {
    int i = 0;
    for ( ; i < nb_criterion ; i++) {
      if (!select_attribute(file, crit[i], cond[i], param[i]))
        break;
      }
    while (i == nb_criterion) {
      int x = add_file_on_list(list, list_length, file);
      if (x == -1) {
        list = realloc(list, 2*list_length);
        if (list == NULL)
          return NULL;
      }
      else {
        list_length += x;
        i--; // The file has been added, we leave the while.
      }
    }
    file = file->next;
  }
  // Frees all mallocs done in the function.
  free_look(buffer, crit, cond, param, NULL, nb_criterion);
  if (list_length > strlen("list [")) {
    list[list_length - 1] = ']';
    list[list_length] = '\n';
    list[list_length+1] = 0;
  } else {
    list[list_length] = ']';
    list[list_length+1] = '\n';
    list[list_length + 2] = 0;
  }
  if(verbose) {
    printf("   End of search.\n");
    printf("   End of request look.\n");
  }
  return list;
}

int update(char* request, char* ip_adr_request, int port, bool verbose) {
  if(verbose) {
    printf("   Processing request update of client %s.\n", ip_adr_request);
  }
  user_t* u = NULL;
  if(verbose) {
    printf("   Searching corresponding user ...\n");
  }
  find_user(port, ip_adr_request, &u);
  if (u == NULL) {
    if(verbose) {
      printf("   ERROR : User (port : %d, ip : %s) not found in update.\n",
            port, ip_adr_request);
    }
    return -1; //erreur, utilisateur non enregistré
  }
  if(verbose) {
    printf("   User collected.\n");
    printf("   Updating files ...\n");
  }
  linked_file_t* lf = u->owned_file;
  while (lf != NULL) {
    delete_linked_user(&(lf->f->owned_by), u);
    lf = lf->next;
  }
  free_all_linked_file(&u->owned_file);
  char buffer[100];
  if (!next_word(&request, buffer, verbose)) //seed
    return 1;
  file_t* f = NULL;
  while (!strcmp(buffer, "leech") && !strcmp(buffer, "")) {
    if (!next_word(&request, buffer, verbose)) //keys
      return -1;
    find_file(buffer, &f);
    new_linked_file(&(u->owned_file), f);
    new_linked_user(&(f->owned_by), u);
  }
  lf = u->leeching;
  while (lf != NULL) {
    delete_linked_user(&(lf->f->leeched_by), u);
    lf = lf->next;
  }
  free_all_linked_file(&(u->leeching));
  if (strcmp(buffer, "leech")) {
    if(verbose) {
      printf("   End of updating files.\n");
      printf("   End of request update.\n");
    }
    return 0;
  } else {
    while (next_word(&request, buffer, verbose)) {
      find_file(buffer, &f);
      new_linked_file(&(u->leeching), f);
      new_linked_user(&(f->leeched_by), u);
    }
    if(verbose) {
      printf("   End of updating files.\n");
      printf("   End of request update.\n");
    }
    return 0;
  }
}
