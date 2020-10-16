#include "processing.h"

int request_identification(char * request, char * response, char * ip, int port, bool verbose) {
  if(verbose) {
    printf("Identification and processing request ...\n");
  }
  char word[SIZE_WORD];

  // Case Empty request or too longue word
  if (!next_word(&request, word, verbose)) {
    return -1;
  }

  if (!strcmp(word, "announce")) {
    if (announce(request, ip, port, verbose)) {
      return -1;
    }
    strcpy(response, "ok\n");
    if (verbose) {
      printf("End of request processing.\n");
    }
    return 0;
  }
  else if (!strcmp(word, "getfile")) {
    char* ret = get_file(request, verbose);
    if(ret == NULL) {
      return -1;
    }
    strcpy(response, ret);
    free(ret);
    if (verbose) {
      printf("End of request processing.\n");
    }
    return 0;
  }
  else if (!strcmp(word, "update")) {
    if (update(request, ip, port, verbose)) {
      return -1;
    }
    strcpy(response, "ok\n");
    if (verbose) {
      printf("End of request processing.\n");
    }
    return 0;
  }
  else if (!strcmp(word, "look")) {
    char* ret = look(request, verbose);
    if (ret == NULL) {
      return -1;
    }
    strcpy(response, ret);
    free(ret);
    if (verbose) {
      printf("End of request processing.\n");
    }
    return 0;
  }
  else {
    fprintf(stderr, "Error : |%s| is an unindentified request.\n", word);
    return -1;
  }
  if (verbose) {
    printf("End of request processing.\n");
  }
  return 0;
}
