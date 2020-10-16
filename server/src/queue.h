#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE_DESCRIPTOR 100
#define SIZE_IP 16

extern struct user* server_user_memory;
extern struct file* server_file_memory;

struct file {
  char name[25];
  int length;
  int piece_size;
  char descriptor[100];
  struct linked_user * owned_by;
  struct linked_user * leeched_by;
  struct file * next;
};

typedef struct file file_t;

struct user {
  int port_serv;
  int port_cli;
  char IP_adr[SIZE_IP];
  struct linked_file* owned_file;
  struct linked_file* leeching;
  struct user* next;
};

typedef struct user user_t;

struct linked_file {
  struct file* f;
  struct linked_file* next;
};

typedef struct linked_file linked_file_t;

struct linked_user {
  struct user* u;
  struct linked_user* next;
};

typedef struct linked_user linked_user_t;


/*
 * Initializes a new struct file. Returns true in case of
 * success and false else.
 */
bool new_file(char* name, int length, int piece_size, char* descriptor);

/*
 * Compares each file stored in server_file_memory and returns
 * true if one of them has same file descriptor as the descriptor
 * given. If one file is found, its address is stored in fi.
 */
bool find_file(char needed_descriptor[SIZE_DESCRIPTOR], file_t** fi);

//bool delete_file(struct file * f); //not_implemented

/*
 * Frees all files contained in server_file_memory and all
 * mallocs made by substructures.
 */
bool free_all_file();

/*
 * Initializes a new struct user. Returns true in case of
 * success and false else.
 */
bool new_user(int port_serv, int port_cli, char ip[SIZE_IP]);

/*
 * Compares each user stored in server_user_memory and returns
 * true if one of them has same part and ip address as those
 * given. If one user is found, its address is stored in us.
 */
bool find_user(int port_serv, char ip[SIZE_IP], user_t** us);

bool delete_user(struct user * u);

/*
 * Frees all files contained in server_file_memory and all
 * mallocs made by substructures.
 */
bool free_all_user();

/*
 * Initialises a new linked_file structure. Stores it at the end of the
 * linked file lf. If f is already in the linked file, do nothing. Returns
 * true in case of success and false else.
 */
bool new_linked_file(linked_file_t** lf, file_t* f);

/*
 * Finds if a file is in the linked file. Compares the pointed values.
 * Returns true if there was a match, else false.
 */
bool find_linked_file(linked_file_t* lf, file_t* f);

/*
 * Deletes an element f in the linked file lf. Returns true in case of
 * sucess, else if the element is not in the linked file.
 */
bool delete_linked_file(linked_file_t** lf, file_t* f);

/*
 * Frees all linked_file contained in the linked file lf. Do
 * not free files. Frees the element remove.
 */
bool free_all_linked_file(linked_file_t** lf);

/*
 * Initialises a new linked_user structure. Stores it at the end of the
 * linked file lu. If u is already in the linked file, do nothing. Returns
 * true in case of success and false else.
 */
bool new_linked_user(linked_user_t** lu, user_t* u);

/*
 * Finds if a file is in the linked file. Compares the pointed values.
 * Returns true if there is a match, else false.
 */
bool find_linked_user(linked_user_t* lu, user_t* u);

/*
 * Deletes an element u in the linked user lu. Returns true in case of
 * sucess, else if the element is not in the linked user.
 */
 bool delete_linked_user(linked_user_t** lu, user_t* u);

/*
 * Frees all linked_user contained in the linked user lu. Do
 * not free users. Frees the element remove.
 */
bool free_all_linked_user(linked_user_t** lu);

#endif
