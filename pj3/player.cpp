#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "socket_lib.h"

using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 3) {
    cout << "Syntax: player <machine_name> <port_num>" << endl;
    exit(EXIT_FAILURE);
  }

  if (argv_number_check(argv, 2, "port_num")) {
    return 1;
  }
    
  if (argc < 2) {
      cout << "Syntax: client <hostname>\n" << endl;
      return 1;
  }

  const char *hostname = argv[1];
  const char *port     = argv[2];

  // get a socket connecting to ringmaster
  int socket_fd_ringmaster = build_client(hostname, port);
  int id = -1;
  int total_players = -1;

  // receive setup messages from ringmaster
  recv(socket_fd_ringmaster, &id, sizeof(id), 0);
  recv(socket_fd_ringmaster, &total_players, sizeof(total_players), 0);
  
  cout << "Connected as player " << id << " out of " << total_players << " total players" << endl;

  // get the hostname and port for this process as server
  char right_server_port[5];
  int use_port = 2512 + id;
  sprintf(right_server_port, "%d", use_port);
  
  char my_hostname[128];
  gethostname(my_hostname, 128);

  cout << my_hostname << endl;
  cout << right_server_port << endl;
  
  // build a server socket and send info to ringmaster
  int right_server_fd = build_server(right_server_port);
  //  send(right_server_fd, my_hostname, strlen(my_hostname)+1, 0);  // hostname
  send(right_server_fd, right_server_port, 5, 0);  // port number

  // game ends, close sockets
  close(socket_fd_ringmaster);
  close(right_server_fd);

  return 0;
}
