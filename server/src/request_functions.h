#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#ifndef REQUEST_FUNCTIONS_H
#define REQUEST_FUNCTIONS_H

#define SIZE_ADDRESS 30

/*
 * This function interprets the keyword "announce". Adds to tracker the
 * new user and all its files. Returns -1 in case of error, else 0.
 * Usage : < announce listen $Port seed [$Filename1 $Length1 $PieceSize1 $Key1
 *         $Filename2 $Length2 $PieceSize2 $Key2 …] leech [$Key3 $Key4 …]
 *         > ok
 * Example : < announce listen 2222 seed [file_a.dat 2097152 1024 8905e92af
 *                                        file_b.dat 3145728 1536 330a57722]
 *           > ok
 */
int announce(char* request, char* ip, int port_serv, bool verbose);

/*
 * This function interprets the keyword "getfile". Returns all pairs
 * that have the file asked.
 * Usage :  < getfile $Key
 *          > peers $Key [$IP1:$Port1 $IP2:$Port2 …]
 * Example : < getfile 8905e92afeb80
 *           > peers 8905e92afeb80 [1.1.1.2:2222 1.1.1.3:3333]
 */
char* get_file(char* request, bool verbose);

/*
 * This function interprets the keyword "look". Returns all files that
 * holds all criterions. Do nothing if there is a problem.
 * Usage : < look [$Criterion1 $Criterion2 …]
 *         > list [$Filename1 $Length1 $PieceSize1 $Key1
 *                 $Filename2 $Length2 $PieceSize2 $Key2 …]
 * Example : < look [filename="file_a.dat" filesize>"1048576"]
 *           > list [file_a.dat 2097152 1024 8905e92afeb80fc7]
 */
char* look(char* request, bool verbose);

/*
 * This functions interprets the keyword "update". Returns 0 if
 * all files have been updated, 1 else.
 * Usage : < update seed [$Key1 $Key2 $Key3 …] leech [$Key10 $Key11 $Key12 …]
 *         > ok
 * Example : < update seed [] leech [8905e92afeb80fc7722ec89eb0bf0966]
 *           > ok
 */
int update(char* request, char* ip_adr_request, int port, bool verbose);

#endif
