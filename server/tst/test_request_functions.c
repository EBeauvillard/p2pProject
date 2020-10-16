#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../src/request_functions.h"

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
  new_file("file_a.dat", 124587, 1024, "8905e92af");
  new_file("file_b.dat", 2097152, 512, "54eaf9621");
  new_file("file_c.dat", 2235479, 256, "ac3258d9");
  new_file("file_d.dat", 1247, 1024, "eeef3524a");
  new_file("file_e.dat", 3658442, 512, "14e985f63");
}

bool user_compare(user_t* user, char IP_adr[20], int port_serv, int port_cli) {
  if(user == NULL)
    return false;
  if(false && strcmp(user->IP_adr, "toto"))
    return false;
  if(user->port_cli != port_cli)
    return false;
  if(user->port_serv != port_serv)
    return false;
  return true;
}

bool file_compare(file_t* owned_file, char name[25], int length, int piece_size, char descriptor[100]) {
  if (owned_file == NULL)
    return false;
  if (strcmp(owned_file->name, name))
    return false;
  if (owned_file->length != length)
    return false;
  if (owned_file->piece_size != piece_size)
    return false;
  if (strcmp(owned_file->descriptor, descriptor))
    return false;
  return true;
}

bool test_announce() {
  char request[200] = "listen 2222 seed [file_a.dat 2097152 1024 8905e92afeb80fc7722ec89eb0bf0966 file_b.dat 3145728 1536 330a57722ec8b0bf09669a2b35f88e9e]";
  int ret = announce(request, "tototot", 65421, false);
  if (ret != 0) {
    return false;
  }
  user_t* user = server_user_memory;

  if (user == NULL) {
    return false;
  }

  if(!user_compare(user, "toto", 65421, 2222)) {
    return false;
  }

  if(user->next != NULL) {
    return false;
  }

  file_t* owned_file = user->owned_file->f;
  if(!file_compare(owned_file, "file_a.dat", 2097152, 1024, "8905e92afeb80fc7722ec89eb0bf0966")) {
    return false;
  }

  owned_file = user->owned_file->next->f;
  if(!file_compare(owned_file, "file_b.dat", 3145728, 1536, "330a57722ec8b0bf09669a2b35f88e9e")) {
    return false;
  }

  struct linked_file * owned_file_link = user->owned_file->next->next;

  if(owned_file_link != NULL) {
    return false;
  }
  free_all_file();
  free_all_user();
  return true;
}

bool test_get_file() {
  char request[200] = "listen 2222 seed [file_a.dat 2097152 1024 8905e92afeb80fc7722ec89eb0bf0966 file_b.dat 3145728 1536 330a57722ec8b0bf09669a2b35f88e9e]";
  announce(request, "tototot", 65879, false);
  char* file = get_file("8905e92afeb80fc7722ec89eb0bf0966", false);
  if(strcmp(file, "peers 8905e92afeb80fc7722ec89eb0bf0966 [tototot:2222]\n"))
    return false;
  free_all_file();
  free_all_user();
  free(file);
  return true;
}

bool test_look() {
  generate_file();
  char* list = look("filename=\"file_a.dat\"", false);
  if (strcmp(list, "list [file_a.dat 124587 1024 8905e92af]\n"))
    return false;
  free(list);
  list = look("filename=\"file_x.dat\"", false);
  if (strcmp(list, "list []\n"))
    return false;
  free(list);
  list = look("filesize>\"1000000\"", false);
  if (strcmp(list, "list [file_b.dat 2097152 512 54eaf9621 file_c.dat 2235479 256 ac3258d9 file_e.dat 3658442 512 14e985f63]\n"))
    return false;
  free(list);
  list = look("filename!=\"file_c.dat\" filename!=\"file_b.dat\" filename!=\"file_e.dat\"", false);
    if (strcmp(list, "list [file_a.dat 124587 1024 8905e92af file_d.dat 1247 1024 eeef3524a]\n"))
      return false;
  free(list);
  list = look("key<\"13eaf9621\"", false);
  if (strcmp(list, "list [file_c.dat 2235479 256 ac3258d9]\n"))
    return false;
  free(list);
  free_all_file();
  return true;
}

bool test_update() {
  char request[200] = "listen 2222 seed [file_a.dat 2097152 1024 8905e92afeb80fc7722ec89eb0bf0966 file_b.dat 3145728 1536 330a57722ec8b0bf09669a2b35f88e9e]";
  announce(request, "tototot", 68954, false);
  char request2[200] = "seed [8905e92afeb80fc7722ec89eb0bf0966]";
  update(request2, "tototot", 68954, false);
  char * get_f=get_file("330a57722ec8b0bf09669a2b35f88e9e", false);
  if (strcmp(get_f, "peers 330a57722ec8b0bf09669a2b35f88e9e []\n"))
    return false;
  free_all_file();
  free_all_user();
  free(get_f);
  return true;
}

int main(int argc, char* argv[]) {
  printf(COLOR_BOLD_BLUE "Tests on the request functions ------------\n" COLOR_RESET);
  printf("Test announce : ");
  display_result(test_announce());

  printf("Test get_file : ");
  display_result(test_get_file());

  printf("Test look : ");
  display_result(test_look());

  printf("Test update : ");
  display_result(test_update());

  return EXIT_SUCCESS;
}
