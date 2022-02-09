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

  //cout << my_hostname << endl;
  cout << "My server port is " << right_server_port << endl;
  
  // build a server socket
  int right_server_fd = build_server(right_server_port);

  // send server information to ringmaster
  send(socket_fd_ringmaster, my_hostname, strlen(my_hostname), 0);  // hostname
  send(socket_fd_ringmaster, right_server_port, strlen(right_server_port), 0);  // port number

  
  // receive left neighbor server info from ringmaster
  
  char left_player_hostname[128];
  char left_player_port[6];
  int n;
  n = recv(socket_fd_ringmaster, left_player_hostname, 128, 0);
  left_player_hostname[n] = 0;
  cout << "Receive left neighbor hostname: " << left_player_hostname << endl;

  n = recv(socket_fd_ringmaster, left_player_port, 6, 0);
  cout << "Receive port number size is " << n << endl;
  left_player_port[n] = 0;

  cout << left_player_hostname << endl;
  cout << left_player_port << endl;
  
  /*
  char left_player_addr[128];
  int n ;
  n = recv(socket_fd_ringmaster, left_player_addr, 128, 0);
  left_player_addr[n] = 0;
  cout << "Receviced: " << left_player_addr << endl;
  */
  /*
  int msg = 1;
  send(socket_fd_ringmaster, &msg, sizeof(msg), 0);
  */
  
  // game ends, close sockets
  n = recv(socket_fd_ringmaster, NULL, 0, 0);
  if (n == 0) {
    close(socket_fd_ringmaster);
    close(right_server_fd);
  }

  return 0;
}
