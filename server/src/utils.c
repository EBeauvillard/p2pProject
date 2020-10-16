#include "utils.h"

bool useless_symbols(char symbol) {
  switch (symbol) {
    case '[':
    case ']':
      return true;
    default:
      return false;
  }
}

bool next_word(char** request, char* word, bool verbose) {
  int i = 0;
  while ((**request == ' ') || (**request == '\r') ||
         (**request == '\n'))
    *request = *request + 1;

  while ((**request != ' ') && (**request != '\r') &&
         (**request != '\n') && (**request != 0)) {
    if (!useless_symbols(**request)) {
      if (i == SIZE_WORD) {
        if(verbose) {
          printf("Word too long (more than %d characters).\n", SIZE_WORD);
        }
        return false;
      }
      word[i] = **request;
      i++;

    }
    *request = *request + 1;
  }
  while (**request == ' ')
    *request = *request + 1;

  word[i] = 0;
  if (word[0] == 0) {
  return false;
  }
  return true;
}

bool is_cond(char* buffer) {
  switch (buffer[0]) {
    case '<':
    case '>':
      return ((buffer[1] == 0) || (buffer[1] == '=') ||
          isalpha(buffer[1]) || isdigit(buffer[1]) ||
          (buffer[1] == '\"'));
    case '=':
      return ((buffer[1] == 0) || isalpha(buffer[1]) ||
              isdigit(buffer[1]) || (buffer[1] == '\"'));
    case '!':
      if (buffer[1] == '=')
        return true;
  }
  return false;
}

int move_cond(char* cond, char* buffer) {
  if (!is_cond(buffer))
    return -1;
  cond[0] = buffer[0];
  if (buffer[1] == '=') {
    cond[1] = '=';
    cond[2] = 0;
    return 2;
  }
  cond[1] = 0;
  return 1;
}

long long int convert_hexadecimal(char* hex) {
  int len = strlen(hex);
  long long int x;
  long long int val = 0;
  long long int b = 1;
  for(int i = len - 1 ; i >= 0 ; i--) {
    if(isdigit(hex[i]))
      x = hex[i] - 48;
    else if ((hex[i] >= 'a') && (hex[i] <= 'f'))
      x = hex[i] - 97 + 10;
    else {
      fprintf(stderr, "The number %s is not in hexadecimal.\n", hex);
      exit(1);
    }
    x *= b;
    val += x;
    b *= 16;
  }
  return val;
}

bool compare_int(long long int a, long long int b, char cond[SIZE_MAX_OPERATOR]) {
  if (!strcmp(cond, "="))
    return a == b;
  if (!strcmp(cond, ">"))
    return a > b;
  if (!strcmp(cond, "<"))
    return a < b;
  if (!strcmp(cond, ">="))
    return a >= b;
  if (!strcmp(cond, "<="))
    return a <= b;
  if (!strcmp(cond, "!="))
    return a != b;

  fprintf(stderr, "Error : operator %s unknown.", cond);
  fprintf(stderr, "Keywords : =, >, <, >=, <=, !=.\n");
  exit(1);
}

bool compare_string(char* a, char* b, char cond[SIZE_MAX_OPERATOR]) {
  if (!strcmp(cond, "="))
    return !strcmp(a, b);
  if (!strcmp(cond, ">"))
    return strcmp(a, b) > 0;
  if (!strcmp(cond, "<"))
    return strcmp(a, b) < 0;
  if (!strcmp(cond, ">="))
    return strcmp(a, b) >= 0;
  if (!strcmp(cond, "<="))
    return strcmp(a, b) <= 0;
  if (!strcmp(cond, "!="))
    return strcmp(a, b) != 0;

  fprintf(stderr, "Error : operator %s unknown.", cond);
  fprintf(stderr, "Keywords : =, >, <, >=, <=, !=.\n");
  exit(1);
}

bool select_attribute(file_t* file, char* crit, char* cond, char* param) {
  if (!strcmp(crit, "filename"))
    return compare_string(file->name, param, cond);
  if (!strcmp(crit, "filesize")) {
    return compare_int(file->length, atoi(param), cond);
  }
  if (!strcmp(crit, "piecesize")) {
    return compare_int(file->piece_size, atoi(param), cond);
  }
  if (!strcmp(crit, "key")) // key is the same as descriptor ?
    return compare_int(convert_hexadecimal(file->descriptor),
                       convert_hexadecimal(param), cond);

  fprintf(stderr, "Error : criterion %s unknown.\n", crit);
  fprintf(stderr, "Keywords : filename, filesize, piecesize, key.\n");
  exit(1);
}

int add_word(char* list, char* word, int list_length) {
  int i = 0;
  while (word[i] != 0) {
    list[list_length + i] = word[i];
    if ( list_length + i >= SIZE_MAX_LIST)
      return -1;
    i++;
  }
  list[list_length + i] = ' ';
  if ( list_length + i >= SIZE_MAX_LIST)
    return -1;
  i++;
  return i;
}

int add_file_on_list(char* list, int list_length, file_t* file) {
  char buffer[SIZE_BUFFER];
  int char_printed = 0;
  int ret;
  // Adds name
  ret = add_word(list, file->name, list_length);
  if (ret == -1)
    return -1;
  char_printed += ret;

  // Adds length
  sprintf(buffer, "%d", file->length);
  ret = add_word(list, buffer, list_length + char_printed);
  if (ret == -1)
    return -1;
  char_printed += ret;

  // Adds piece_size
  sprintf(buffer, "%d", file->piece_size);
  ret = add_word(list, buffer, list_length + char_printed);
  if (ret == -1)
    return -1;
  char_printed += ret;

  // Adds key
  ret = add_word(list, file->descriptor, list_length + char_printed);
  if (ret == -1)
    return -1;
  char_printed += ret;

  return char_printed;
}

int split_criterion(char* crit, char* cond, char* param, char* buffer) {
  int j = 0;
  int i = 0;
  // Handle crit
  while (isalpha(buffer[i])) {
    crit[j] = buffer[i];
    j++;
    i++;
  }
  crit[j] = 0;

  // Handle cond
  int c = move_cond(cond, buffer+i);
  if (c == -1)
    return -1;
  i += c;

  // Handle param
  j = 0;
  i++; // Deleting first "
  while (buffer[i] != '\"') {
    param[j] = buffer[i];
    i++;
    j++;
  }

  param[j] = 0;
  return 0;
}

int malloc_look(char** buffer, char*** crit, char*** cond,
                char*** param, char** list) {
  *buffer = malloc(sizeof(char) * SIZE_BUFFER);
  *crit = malloc(sizeof(char*) * NB_MAX_CRITERION);
  *cond = malloc(sizeof(char*) * NB_MAX_CRITERION);
  *param = malloc(sizeof(char*) * NB_MAX_CRITERION);
  *list = malloc(sizeof(char) * SIZE_MAX_LIST);
  if ((*buffer == NULL) || (*crit == NULL) || (*cond == NULL) ||
      (*param == NULL) || (*list == NULL)) {
    fprintf(stderr, "Error : impossible to malloc datas for look.\n");
    return -1;
  }
  return 0;
}

int malloc_criterion(char** crit, char** cond, char** param) {
  *crit = malloc(sizeof(char) * SIZE_BUFFER);
  *cond = malloc(sizeof(char) * SIZE_MAX_OPERATOR);
  *param = malloc(sizeof(char) * SIZE_BUFFER);
  if ((*crit == NULL) || (*cond == NULL) || (*param == NULL)) {
    fprintf(stderr, "Error : impossible to malloc datas for criterion.\n");
    return -1;
  }
  return 0;
}

void free_look(char* buffer, char** crit, char** cond,
               char** param, char* list, int nb_criterion) {
  for (int i = 0 ; i < nb_criterion ; i++) {
    free(crit[i]);
    free(cond[i]);
    free(param[i]);
    }
    free(crit);
    free(cond);
    free(param);
    free(buffer);

    if (list != NULL)
      free(list);
}
