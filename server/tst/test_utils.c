#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../src/utils.h"

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

bool test_next_word_empty() {
  char* text = "";
  char next[64];
  int ret;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "")) || (ret != false))
    return false;
  return true;
}

bool test_next_word_simple() {
  char* text = "announce listen";
  char next[64];
  bool ret;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "announce")) || (ret != true))
    return false;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "listen")) || (ret != true))
    return false;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "")) || (ret != false))
    return false;
  return true;
}

bool test_next_word_useless_symbol_begin() {
  char* text = "seed [key";
  char next[64];
  bool ret;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "seed")) || (ret != true))
    return false;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "key")) || (ret != true))
    return false;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "")) || (ret != false))
    return false;
  return true;
}

bool test_next_word_useless_symbol_end() {
  char* text = "seed key]";
  char next[64];
  bool ret;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "seed")) || (ret != true))
    return false;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "key")) || (ret != true))
    return false;
  ret = next_word(&text, next, false);
  if ((strcmp(next, "")) || (ret != false))
    return false;
  return true;
}

bool test_is_cond() {
  char* tests[9] = { "<", ">", "<=", ">=", "=", "!=", "==", "qd", "!!" };
  bool results[9] = { true, true, true, true, true, true, false, false, false };
  for (int i = 0 ; i < 9 ; i++)
    if (is_cond(tests[i]) != results[i])
      return false;
  return true;
}

bool test_move_cond_with_equal() {
  char* buffer = "=145786";
  char cond[2];
  if (move_cond(cond, buffer) != 1)
    return false;
  if (strcmp(cond, "="))
    return false;
  return true;
}

bool test_move_cond_with_different() {
  char* buffer = "!=145786";
  char cond[2];

  if (move_cond(cond, buffer) != 2)
    return false;
  if (strcmp(cond, "!="))
    return false;
  return true;
}

bool test_move_cond_with_no_symbol() {
  char* buffer = "145786";
  char cond[2];
  if (move_cond(cond, buffer) != -1)
    return false;
  return true;
}

bool test_convert_hexadecimal() {
  char* hex = "64";
  if (convert_hexadecimal(hex) != 100)
    return false;
  return true;
}

bool test_compare_int() {
  if (compare_int(147, 132, "<"))
    return false;
  if (!compare_int(128, 128, "="))
    return false;
  if (compare_int(18, 18, "!="))
    return false;
  if (compare_int(18, 19, "="))
    return false;
  if (!compare_int(28, 28, ">="))
    return false;
  if (!compare_int(100, 40, ">="))
    return false;
  return true;
}

bool test_compare_string() {
  if (!compare_string("tata", "toto", "<"))
    return false;
  if (!compare_string("toto", "toto", "="))
    return false;
  if (compare_string("toto", "toto", "!="))
    return false;
  if (compare_string("toto", "tata", "="))
    return false;
  if (!compare_string("truc", "truc", ">="))
    return false;
  if (!compare_string("truc", "trac", ">="))
    return false;
  return true;
}

bool test_select_attribute() {
  new_file("toto", 64, 28, "af856c69d");
  file_t* file = server_file_memory;
  if (!select_attribute(file, "filename", "=", "toto"))
    return false;
  if (select_attribute(file, "filesize", ">", "70"))
    return false;
  if (select_attribute(file, "piecesize", "!=", "28"))
    return false;
  if (select_attribute(file, "key", ">", "af856c69d"))
    return false;
  free_all_file();
  return true;
}

bool test_add_word_empty_list() {
  char list[100];
  if (add_word(list, "azerty", 0) != 7)
    return false;
  list[7] = 0; // Because we can't apply strcmp else.
  if (strcmp(list, "azerty "))
    return false;
  return true;
}

bool test_add_word_normal() {
  char list[100] = "toto";
  if (add_word(list, "azerty", 4) != 7)
    return false;
  list[12] = 0; // Because we can't apply strcmp else.
  if (strcmp(list, "totoazerty "))
    return false;
  return true;
}

bool test_add_file_on_list() {
  char list[100] = "file";
  new_file("toto", 64, 28, "azertyuiop");
  file_t* file = server_file_memory;
  if (add_file_on_list(list, 4, file) != 22)
    return false;
  list[26] = 0; // Because we can't apply strcmp else.
  if (strcmp(list, "filetoto 64 28 azertyuiop "))
    return false;
  free_all_file(file);
  return true;
}

bool test_split_criterion_empty() {
  char crit[100];
  char cond[3];
  cond[3] = 0;  // Because we can't apply strcmp else.
  char param[100];
  char* buffer = "";

  if (split_criterion(crit, cond, param, buffer) != -1)
    return false;

  return true;
}

bool test_split_criterion_normal_case() {
  char crit[100];
  char cond[3];
  cond[2] = 0;
  cond[3] = 0;  // Because we can't apply strcmp else.
  char param[100];
  char* buffer = "namefile=\"toto\"";

  if (split_criterion(crit, cond, param, buffer) != 0)
    return false;
  if (strcmp(crit, "namefile") || strcmp(cond, "=") || strcmp(param, "toto"))
    return false;

  buffer = "sizefile!=\"67\"";
  if (split_criterion(crit, cond, param, buffer) != 0)
    return false;

  if (strcmp(crit, "sizefile") || strcmp(cond, "!=") || strcmp(param, "67"))
    return false;

  buffer = "sizefile==\"24\"";
  if (split_criterion(crit, cond, param, buffer) != -1)
    return false;

  return true;
}

int main(int argc, char* argv[]) {
  printf(COLOR_BOLD_BLUE "Tests on the utils functions --------------\n" COLOR_RESET);
  printf("Test next_word empty : ");
  display_result(test_next_word_empty());

  printf("Test next_word simple : ");
  display_result(test_next_word_simple());

  printf("Test next_word with useless symbol [ : ");
  display_result(test_next_word_useless_symbol_begin());

  printf("Test next_word with useless symbol ] : ");
  display_result(test_next_word_useless_symbol_end());

  printf("Test is_cond : ");
  display_result(test_is_cond());

  printf("Test move_cond with = : ");
  display_result(test_move_cond_with_equal());

  printf("Test move_cond with !=: ");
  display_result(test_move_cond_with_different());

  printf("Test move_cond with no symbol: ");
  display_result(test_move_cond_with_no_symbol());

  printf("Tests the convertion from hexadecimal to decimal : ");
  display_result(test_convert_hexadecimal());

  printf("Test compare with int: ");
  display_result(test_compare_int());

  printf("Test compare with strings: ");
  display_result(test_compare_string());

  printf("Test select_attribute : ");
  display_result(test_select_attribute());

  printf("Test add_word on empty list : ");
  display_result(test_add_word_empty_list());

  printf("Test add_word in a normal case : ");
  display_result(test_add_word_normal());

  printf("Test add_file_on_list : ");
  display_result(test_add_file_on_list());

  printf("Test split_criterion on an empty criterion : ");
  display_result(test_split_criterion_empty());

  printf("Test split_criterion in a normal case : ");
  display_result(test_split_criterion_normal_case());

  return EXIT_SUCCESS;
}
