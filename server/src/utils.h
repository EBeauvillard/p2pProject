//Functions used globally
#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "queue.h"

#define NB_MAX_CRITERION 20
#define SIZE_BUFFER 100
#define SIZE_MAX_OPERATOR 3
#define SIZE_MAX_LIST 1024
#define SIZE_WORD 100

/*
 * Check if a symbol is useful or not.
 * Possibly obtained from requests  ... seed [filename length ... key].
 */
bool useless_symbols(char symbol);

/*
 * This functions gets the first word contained in request by
 * removing it and storing it in word. Removes also spaces.
 * Returns true if a word has been removed, false else.
 */
bool next_word(char** request, char* word, bool verbose);

/*
 * Indicates if the beginning of buffer is a conditional symbol.
 * Conditional symbols : <, >, <=, >=, =, !=
 */
bool is_cond(char* buffer);

/*
 * This function gets the conditional buffer at the beginning of buffer,
 * and storing it to cond.
 * Returns -1 if the beginning of the buffer is not a conditional symbol
 * and the size of the symbol else (1 or 2).
 */
int move_cond(char* cond, char* buffer);

/*
 * Converts a string containing an hexadecimal number in a decimal
 * number.
 */
long long int convert_hexadecimal(char* hex);

/*
 * Given two ints, checks if a cond b is true. If cond is not defined,
 * exits the program.
 */
bool compare_int(long long int a, long long int b, char cond[SIZE_MAX_OPERATOR]);

/*
 * Given two strings, checks if a cond b is true. If cond is not defined,
 * exits the program.
 */
bool compare_string(char* a, char* b, char cond[SIZE_MAX_OPERATOR]);

/*
 * Selects the attribute to compare in the structure file with
 * keyword in crit. If crit is not defined, exits the program.
 */
bool select_attribute(file_t* file, char* crit, char* cond, char* param);

/*
 * Writes word at the end of the list. Returns the number of
 * characters printed and -1 if the file has been overflow.
 */
int add_word(char* list, char* word, int list_length);

/*
 * Prints all caracteristics of a file at the end of a list. Returns
 * the number of char printed and -1 if the list_size has been
 * overflow.
 */
int add_file_on_list(char* list, int list_length, file_t* file);

/*
 * This function splits a criterion in three parts : crit, cond and param.
 * Returns -1 if it's not a criterion, else 0.
 * Criterion : crit|cond|param   => filename="file_a.dat"
 */
int split_criterion(char* crit, char* cond, char* param, char* buffer);

/*
 * Malloc all elements used in the function look. Returns -1 if there is
 * an arror, 0 else.
 */
int malloc_look(char** buffer, char*** crit, char*** cond,
                char*** param, char** list);

/*
 * Malloc elements used to stock a criterion. Returns -1 if there is
 * an error, 0 else.
 */
int malloc_criterion(char** crit, char** cond, char** param);

/*
 * Frees all malloc made in the structure. if list is equal to NULL,
 * don't free list.
 */
void free_look(char* buffer, char** crit, char** cond,
               char** param, char* list, int nb_criterion);

#endif
