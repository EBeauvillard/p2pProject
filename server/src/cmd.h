#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef CMD_H
#define CMD_H

extern int port_client;
extern bool verbose;
extern int piece_size;
extern int message_size;
extern int nb_clients_max;

/*
 * Reorganizes the cmd line and looks for -v option.
 */
int parse_opts(int argc, char* argv[]);


#endif
