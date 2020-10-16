#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../src/queue.h"

#define COLOR_BOLD_RED     "\x1b[1;31m"
#define COLOR_BOLD_GREEN   "\x1b[1;32m"
#define COLOR_BOLD_BLUE    "\x1b[1;34m"
#define COLOR_RESET        "\x1b[0m"

void display_result(bool result) {
  if (result)
    printf(COLOR_BOLD_GREEN "Successful\n" COLOR_RESET);
  else
    printf(COLOR_BOLD_RED "FAILURE\n" COLOR_RESET);
}

void generate_file() {
  new_file("toto", 64, 128, "12e58acd32");
  new_file("tutu", 128, 50, "85d7ff63a2");
  new_file("tata", 140, 78, "d58a96baa1");
  new_file("titi", 100, 12, "a2cd6efe61");
  new_file("tete", 20, 140, "159aee2c36");
}

void generate_user() {
  new_user(84321, 8080, "192.168.34.25");
  new_user(94215, 2200, "127.197.35.55");
  new_user(64785, 5500, "255.255.255.255");
  new_user(35751, 3300, "123.98.147.20");
  new_user(78456, 8000, "100.100.435.13");
}

linked_file_t* generate_linked_file() {
  generate_file();
  linked_file_t* lf = NULL;
  new_linked_file(&lf, server_file_memory);
  new_linked_file(&lf, server_file_memory->next);
  new_linked_file(&lf, server_file_memory->next->next->next);
  new_linked_file(&lf, server_file_memory->next->next);
  return lf;
}

linked_user_t* generate_linked_user() {
  generate_user();
  linked_user_t* lu = NULL;
  new_linked_user(&lu, server_user_memory);
  new_linked_user(&lu, server_user_memory->next);
  new_linked_user(&lu, server_user_memory->next->next->next);
  new_linked_user(&lu, server_user_memory->next->next);
  return lu;
}

bool test_new_file() {
  bool file = new_file("toto", 64, 128, "12e58acd32");
  if (!file)
    return false;
  if (strcmp(server_file_memory->name, "toto"))
    return false;
  if (server_file_memory->length != 64)
    return false;
  if (server_file_memory->piece_size != 128)
    return false;
  if (strcmp(server_file_memory->descriptor, "12e58acd32"))
    return false;
  if (server_file_memory->owned_by != NULL)
    return false;
  if (server_file_memory->next != NULL)
    return false;

  file = new_file("tutu", 128, 50, "85d7ff63a2");
  if (!file)
    return false;
  if (server_file_memory->next == NULL)
    return false;
  file_t* f = server_file_memory->next;
  if (strcmp(f->name, "tutu"))
    return false;
  if (f->length != 128)
    return false;
  if (f->piece_size != 50)
    return false;
  if (strcmp(f->descriptor, "85d7ff63a2"))
    return false;
  if (f->owned_by != NULL)
    return false;
  if (f->next != NULL)
    return false;

  file = new_file("tata", 300, 58, "85d7ff63a2");
  if (file)
    return false;
  free_all_file();
  return true;
}

bool test_find_file() {
  generate_file();
  if (!find_file("12e58acd32", NULL))
    return false;
  if (!find_file("a2cd6efe61", NULL))
    return false;
  if (!find_file("159aee2c36", NULL))
    return false;
  if (find_file("tiensf", NULL))
    return false;
  if (find_file("", NULL))
    return false;
  free_all_file();
  return true;
}

bool test_new_user() {
  bool user = new_user(32148, 8080, "192.168.35.25");
  if (!user)
    return false;
  if (server_user_memory->port_cli != 8080)
    return false;
  if (server_user_memory->port_serv != 32148)
    return false;
  if (strcmp(server_user_memory->IP_adr, "192.168.35.25"))
    return false;
  if (server_user_memory->owned_file != NULL)
    return false;
  if (server_user_memory->next != NULL)
    return false;

  user = new_user(24876, 2200, "127.197.35.55");
  if (!user)
    return false;
  if (server_user_memory->next == NULL)
    return false;
  user_t* u = server_user_memory->next;
  if (u->port_cli != 2200)
    return false;
  if (u->port_serv != 24876)
    return false;
  if (strcmp(u->IP_adr, "127.197.35.55"))
    return false;
  if (u->owned_file != NULL)
    return false;
  if (u->next != NULL)
    return false;

  user = new_user(24876, 2200, "127.197.35.55");
  if (user)
    return false;
  free_all_user();
  return true;
}

bool test_find_user() {
  generate_user();
  if (!find_user(84321, "192.168.34.25", NULL))
    return false;
  if (!find_user(35751, "123.98.147.20", NULL))
    return false;
  if (find_user(35478, "123.98.147.20", NULL))
    return false;
  if (find_user(35751, "123.99.147.75", NULL))
    return false;
  if (find_user(69875, "17.16.198.134", NULL))
    return false;
  free_all_user();
  return true;
}

bool test_delete_user() {
  generate_user();
  user_t* previous = server_user_memory->next;
  user_t* u = server_user_memory->next->next;
  user_t* next = server_user_memory->next->next->next;
  if (!delete_user(u))
    return false;
  if (previous->next != next)
    return false;
  u = malloc(sizeof(user_t));
  u->port_serv = 1247;
  u->port_cli = 67845;
  strcpy(u->IP_adr, "165.128.12.45");
  if (delete_user(u))
    return false;
  free(u);
  u = NULL;
  if (delete_user(u))
    return false;
  free_all_user();
  return true;
}

bool test_new_linked_file() {
  linked_file_t* lf1 = NULL;
  new_file("toto", 64, 128, "12e58acd32");
  int linked_file = new_linked_file(&lf1, server_file_memory);
  if (!linked_file)
    return false;
  if (lf1 == NULL)
    return false;
  if (lf1->next != NULL)
    return false;
  if (lf1->f != server_file_memory)
    return false;

  new_file("tutu", 35, 238, "157d2ac6bc");
  linked_file = new_linked_file(&lf1, server_file_memory->next);
  if (!linked_file)
    return false;
  if (lf1->next == NULL)
    return false;
  if (lf1->next->next != NULL)
    return false;
  if (lf1->next->f != server_file_memory->next)
    return false;

  linked_file = new_linked_file(&lf1, server_file_memory->next);
  if (!linked_file)
    return false;

  free_all_linked_file(&lf1);
  free_all_file();
  return true;
}

bool test_find_linked_file() {
  linked_file_t* lf = generate_linked_file();
  if (!find_linked_file(lf, server_file_memory))
    return false;
  if (!find_linked_file(lf, server_file_memory->next))
    return false;
  if (!find_linked_file(lf, server_file_memory->next->next))
    return false;
  if (!find_linked_file(lf, server_file_memory->next->next->next))
    return false;
  if (find_linked_file(lf, server_file_memory->next->next->next->next))
    return false;
  free_all_linked_file(&lf);
  free_all_file();
  return true;
}

bool test_delete_linked_file() {
  linked_file_t* lf = generate_linked_file();
  if (!delete_linked_file(&lf, server_file_memory))
    return false;
  if (delete_linked_file(&lf, server_file_memory))
    return false;
  if (!delete_linked_file(&lf, server_file_memory->next->next))
    return false;
  if (delete_linked_file(&lf, server_file_memory->next->next))
    return false;
  if (delete_linked_file(&lf, server_file_memory->next->next->next->next))
    return false;
  free_all_file();
  free_all_linked_file(&lf);
  return true;
}

bool test_new_linked_user() {
  linked_user_t* lu1 = NULL;
  new_user(8080, 36984, "189.145.125.241");
  int linked_user = new_linked_user(&lu1, server_user_memory);
  if (!linked_user)
    return false;
  if (lu1 == NULL)
    return false;
  if (lu1->next != NULL)
    return false;
  if (lu1->u != server_user_memory)
    return false;

  new_user(3000, 34578, "123.218.214.23");
  linked_user = new_linked_user(&lu1, server_user_memory->next);
  if (!linked_user)
    return false;
  if (lu1->next == NULL)
    return false;
  if (lu1->next->next != NULL)
    return false;
  if (lu1->next->u != server_user_memory->next)
    return false;

  linked_user = new_linked_user(&lu1, server_user_memory->next);
  if (!linked_user)
    return false;

  free_all_linked_user(&lu1);
  free_all_user();
  return true;
}

bool test_find_linked_user() {
  linked_user_t* lu = generate_linked_user();
  if (!find_linked_user(lu, server_user_memory))
    return false;
  if (!find_linked_user(lu, server_user_memory->next))
    return false;
  if (!find_linked_user(lu, server_user_memory->next->next))
    return false;
  if (!find_linked_user(lu, server_user_memory->next->next->next))
    return false;
  if (find_linked_user(lu, server_user_memory->next->next->next->next))
    return false;
  free_all_user();
  free_all_linked_user(&lu);
  return true;
}

bool test_delete_linked_user() {
  linked_user_t* lu = generate_linked_user();
  if (!delete_linked_user(&lu, server_user_memory))
    return false;
  if (delete_linked_user(&lu, server_user_memory))
    return false;
  if (!delete_linked_user(&lu, server_user_memory->next->next))
    return false;
  if (delete_linked_user(&lu, server_user_memory->next->next))
    return false;
  if (delete_linked_user(&lu, server_user_memory->next->next->next->next))
    return false;
  free_all_user();
  free_all_linked_user(&lu);
  return true;
}

int main(int argc, char* argv[]) {
  printf(COLOR_BOLD_BLUE "Tests on the queues functions -------------\n" COLOR_RESET);
  printf("Test new_file : ");
  display_result(test_new_file());

  printf("Test find_file : ");
  display_result(test_find_file());

  printf("Test new_user : ");
  display_result(test_new_user());

  printf("Test find_user : ");
  display_result(test_find_user());

  printf("Test delete_user : ");
  display_result(test_delete_user());

  printf("Test new_linked_file : ");
  display_result(test_new_linked_file());

  printf("Test find_linked_file : ");
  display_result(test_find_linked_file());

  printf("Test delete_linked_file : ");
  display_result(test_delete_linked_file());

  printf("Test new_linked_user : ");
  display_result(test_new_linked_user());

  printf("Test find_linked_user : ");
  display_result(test_find_linked_user());

  printf("Test delete_linked_user : ");
  display_result(test_delete_linked_user());

  return EXIT_SUCCESS;
}
