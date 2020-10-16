#include "queue.h"

struct user* server_user_memory = NULL;
struct file* server_file_memory = NULL;

bool new_file(char* name, int length, int piece_size, char* descriptor) {
  if(find_file(descriptor, NULL)) {
    return false;
  }
  if (strlen(descriptor) >= 100) {
    return false;
  }
  if (strlen(name) >= 25) {
    return false;
  }
  file_t* file;
  if (server_file_memory == NULL) {
    server_file_memory = malloc(sizeof(file_t));
    if (server_file_memory == NULL)
      return NULL;
    file = server_file_memory;
  } else {
    file = server_file_memory;
    while (file->next != NULL)
      file = file->next;
    file->next = malloc(sizeof(file_t));
    if (file->next == NULL)
      return false;
    file = file->next;
  }
  strcpy(file->name, name);
  file->length = length;
  file->piece_size = piece_size;
  strcpy(file->descriptor, descriptor);
  file->owned_by = NULL;
  file->leeched_by=NULL;
  file->next = NULL;
  return true;
}

bool find_file(char needed_descriptor[SIZE_DESCRIPTOR], file_t** fi) {
  file_t* f = server_file_memory;
  while ((f != NULL) && strcmp(f->descriptor, needed_descriptor))
    f = f->next;

  if (f == NULL)
    return false;

  if (fi != NULL)
    *fi = f;

  return true;
}

//int delete_file(struct file * f); //not_implemented

bool free_all_file() {
  file_t* f = server_file_memory;
  file_t* to_free = f;
  while (f != NULL) {
    f = f->next;
    free_all_linked_user(&(to_free->owned_by));
    free(to_free);
    to_free = f;
  }
  server_file_memory = NULL;
  return true;
}

bool new_user(int port_serv, int port_cli, char ip[SIZE_IP]) {
  if (find_user(port_serv, ip, NULL)) {
    return false;
  }
  user_t* user;
  if (server_user_memory == NULL) {
    server_user_memory = malloc(sizeof(user_t));
    if(server_user_memory == NULL) {
      return false;
    }
    user = server_user_memory;
  } else {
    user = server_user_memory;
    while (user->next != NULL)
      user = user->next;
    user->next = malloc(sizeof(user_t));
    if (user->next == NULL)
      return false;
    user = user->next;
  }
  strcpy(user->IP_adr, ip);
  user->port_cli = port_cli;
  user->port_serv = port_serv;
  user->owned_file = NULL;
  user->leeching=NULL;
  user->next = NULL;
  return true;
}

bool find_user(int port_serv, char ip[SIZE_IP], user_t** us) {
  user_t* u = server_user_memory;
  if (port_serv == -1) {
    while ((u != NULL) && strcmp(u->IP_adr, ip))
      u = u->next;
  } else {
    while ((u != NULL) && (strcmp(u->IP_adr, ip) || (u->port_serv != port_serv)))
      u = u->next;
  }

  if (u == NULL)
    return false;

  if (us != NULL)
    *us = u;

  return true;
}

bool delete_user(struct user * u) {
  if(u == NULL) {
    return false;
  }
  if(!find_user(u->port_serv, u->IP_adr, NULL)) {
    return false;
  }
  struct linked_file * lf = u->owned_file;
  while(lf != NULL) {
    struct file * fi = lf->f;
    delete_linked_user(&(fi->owned_by), u);
    lf=lf->next;
  }
  struct user * serv=server_user_memory;
  struct user * prev=NULL;
  while(serv != u) {
    prev=serv;
    serv = serv->next;
  }
  if(prev==NULL) {
    server_user_memory=serv->next;
    free(serv);
  } else {
    prev->next=serv->next;
    free(serv);
  }
  return true;
}

bool free_all_user() {
  user_t* u = server_user_memory;
  user_t* to_free = u;
  while (u != NULL) {
    u = u->next;
    free_all_linked_file(&(to_free->owned_file));
    free(to_free);
    to_free = u;
  }
  server_user_memory = NULL;
  return true;
}

bool new_linked_file(linked_file_t** lf, file_t* f) {
  if (*lf == NULL) {
    *lf = malloc(sizeof(linked_file_t));
    if (*lf == NULL)
      return false;

    (*lf)->f = f;
    (*lf)->next = NULL;
  } else {
    if (find_linked_file(*lf, f)) {
      return true;
    }
    linked_file_t* l = *lf;
    while (l->next != NULL)
      l = l->next;

    l->next = malloc(sizeof(linked_file_t));
    l = l->next;
    if (l == NULL)
      return false;

    l->f = f;
    l->next = NULL;
  }
  return true;
}

bool find_linked_file(linked_file_t* lf, file_t* f) {
  if (lf == NULL)
    return false;

  linked_file_t* l = lf;
  while ((l->next != NULL) && (l->f != f))
    l = l->next;

  if (l->f == f)
    return true;
  else
    return false;
}

bool delete_linked_file(linked_file_t** lf, file_t* f) {
  if (!find_linked_file(*lf, f)) {
    return false;
  }
  linked_file_t* l = *lf;
  linked_file_t* l_previous = NULL;

  while ((l != NULL) && (l->f != f)) {
    l_previous = l;
    l = l->next;
  }
  if (l == NULL)
    return false;

  if (l_previous != NULL)
    l_previous->next = l->next;
  else
    *lf = (*lf)->next;
  free(l);
  return true;
}

bool free_all_linked_file(linked_file_t** lf) {
  if (lf == NULL)
    return false;
  if (*lf == NULL)
    return true;
  linked_file_t* l = *lf;
  linked_file_t* lnext;
  while (l != NULL) {
    lnext = l->next;
    free(l);
    l = lnext;
  }

  *lf = NULL;
  return true;
}

bool new_linked_user(linked_user_t** lu, user_t* u) {
  if (*lu == NULL) {
    *lu = malloc(sizeof(linked_user_t));
    if (*lu == NULL)
      return false;
    (*lu)->u = u;
    (*lu)->next = NULL;
  } else {
    if (find_linked_user(*lu, u))
      return true;
    linked_user_t* l = *lu;
    while (l->next != NULL)
      l = l->next;

    l->next = malloc(sizeof(struct linked_user));
    l = l->next;
    if (l == NULL)
      return false;
    l->u = u;
    l->next = NULL;
  }
  return true;
}

bool find_linked_user(linked_user_t* lu, user_t* u) {
  if(lu==NULL) {
    return false;
  }
  struct linked_user* l = lu;
  while(l->next != NULL && l->u != u) {
    l=l->next;
  }
  if(l->u == u) {
    return true;
  } else {
    return false;
  }
}

bool delete_linked_user(linked_user_t** lu, user_t* u) {
  if (!find_linked_user(*lu, u)) {
    return false;
  }
  linked_user_t* l = *lu;
  linked_user_t* l_previous = NULL;

  while ((l != NULL) && (l->u != u)) {
    l_previous = l;
    l = l->next;
  }
  if (l == NULL)
    return false;

  if (l_previous != NULL)
    l_previous->next = l->next;
  else
    *lu = (*lu)->next;
  free(l);
  return true;
}

bool free_all_linked_user(linked_user_t** lu) {
  if (lu == NULL)
    return false;
  if (*lu == NULL)
    return true;
  linked_user_t* l = *lu;
  linked_user_t* lnext;
  while (l != NULL) {
    lnext = l->next;
    free(l);
    l = lnext;
  }

  *lu = NULL;
  return true;
}
