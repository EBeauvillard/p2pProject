#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "request_functions.h"

#ifndef PROCESSING_H
#define PROCESSING_H

/*
 *
 */
int request_identification(char * request, char* response, char * ip, int port, bool verbose);

#endif
