#include "cmd.h"

// The variable that display all states or only
// those checked true.
int port_client = 3333;
bool verbose = false;
int piece_size = 2048;
int message_size = 16000;
int nb_clients_max = 5;

int parse_opts(int argc, char* argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "c:m:n:p:v")) != -1) {
    switch (opt) {
    case 'c':
      nb_clients_max = atoi(optarg);
      break;
    case 'm':
      message_size = atoi(optarg);
      break;
    case 'n':
      piece_size = atoi(optarg);
      break;
    case 'p':
      port_client = atoi(optarg);
    case 'v':
      verbose = true;
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s nb_port [-v -p piecesize -m messagesize -c nbclientsmax] \n",
              argv[0]);
      exit(EXIT_FAILURE);
      }
    }
  return optind;
}
