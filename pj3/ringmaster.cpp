#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <stdlib.h>
#include <vector>

#include "socket_lib.h"
#include "potato.h"

using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 4) {
    cout << "Syntax: ringmaster <port_num> <num_players> <num_hops>" << endl;
    return 1;
  }

  if (argv_number_check(argv, 1, "port_num") || argv_number_check(argv, 2, "num_players") || argv_number_check(argv, 3, "num_hops")) {
    return 1;
  }
  
  const char *port = argv[1];
  const int num_players = atoi(argv[2]);
  const int num_hops = atoi(argv[3]);

  if (num_players < 1) {
    cout << "player number must be positive" << endl;
    exit(EXIT_FAILURE);
  }

  if (num_hops < 0) {
    cout << "hops number must be non-negative" << endl;
    exit(EXIT_FAILURE);
  }
  
  vector<int> player_socketfd;
  vector<string> player_hostnames;
  vector<string> player_ports;
  
  cout << "Potato Ringmaster" << endl;
  cout << "Players = " << num_players << endl;
  cout << "Hops = " << num_hops << endl;

  // build a server socket which is listening
  int socket_fd = build_server(port);

  // manage players connecting in, allocating their ID to them
  // get and store their hostnames and ports
  for (int i = 0; i < num_players; i++) {
    // accept connection with player
    int connection_socket_fd = server_accept(socket_fd);
    player_socketfd.push_back(connection_socket_fd);

    // send id, total players to connected player
    send(connection_socket_fd, &i, sizeof(i), 0);
    send(connection_socket_fd, &num_players, sizeof(num_players), 0);    

    

    // receive and store address and port of this player
    char player_hostname[128];
    char player_port[6];
    int player_port_num;
    int n = 0;
    n = recv(connection_socket_fd, player_hostname, 128, 0);
    player_hostname[n] = 0;
    n = recv(connection_socket_fd, player_port, 6, 0);
    player_port[n] = 0;
    //string s = to_string(player_port_num);
    //strcpy(player_port, s.c_str());
    

    // for testing
    //cout << "Receive new player hostname: " << player_hostname << endl;
    //cout << "Receive player port: " << player_port << endl;

    // put data into vector
    player_hostnames.push_back(string(player_hostname));
    player_ports.push_back(string(player_port));
  }

  
  // sending neighbouring information to players
  // every player are the server to their right neighbor  
  for (int i = 0; i < num_players; i++) {
    // i is the owner of information(server), pi is the receiver(client)
    int pi = (i+1)%num_players;
    int n;
    int client_fd = player_socketfd[pi];
    string server_hostname = player_hostnames[i];
    string server_port = player_ports[i];

    // convert from string to char *
    char server_hostname_c[server_hostname.length()+1];
    char server_port_c[server_port.length()+1];
    strcpy(server_hostname_c, server_hostname.c_str());
    strcpy(server_port_c, server_port.c_str());
    /*
    char server_addr[server_hostname.length() + server_port.length() + 2];
    strcpy(server_addr, server_hostname.c_str());
    server_addr[server_hostname.length()] = ':';
    strcpy(server_addr+server_hostname.length()+1, server_port.c_str());
    */
    
    //cout << "Ready to send out hostname: " << server_hostname_c << endl;
    //cout << "Ready to send out port: " << server_port_c << endl;
    
    send(client_fd, server_hostname_c, strlen(server_hostname_c), 0);    
    //cout << "Send out hostname: " << server_hostname_c << endl;

    send(client_fd, server_port_c, strlen(server_port_c), 0);
    //cout << "Send out port: " << server_port_c << endl;    
    
    /*
    cout << "Ready to send " << server_addr << endl;
    send(client_fd, server_addr, strlen(server_addr), 0);
    */
  }

  
  // recv ack(1) from every player, print message
  for (int i = 0; i < num_players; i++) {
    int msg;
    int player_fd = player_socketfd[i];    
    recv(player_fd, &msg, sizeof(msg), 0);
    if (msg == 1) {
    cout << "Player " << i << " is ready to play" << endl;
    }
    else {
      cout << "Unexpected message" << endl;
      exit(EXIT_FAILURE);
    }
  }

  // coner case
  if (num_hops == 0) {
    for (int fd: player_socketfd) {
      close(fd);
    }
    close(socket_fd);
    return 0;
  }
  
  // initialize a potato and give it to a random player
  potato p;
  p.left_hops = num_hops-1;
  p.used_hops = 1;
  for (int i = 0; i < POTATO_TRACESIZE; i++) {
    p.trace[i] = -1;
  }
  srand((unsigned int) time(NULL) + num_players);
  int random = rand() % num_players;
  send(player_socketfd[random], &p, sizeof(p), 0);
  cout << "Ready to start the game, sending potato to player " << random << endl;

  // receive potato from player, print trace and give it back to them
  // mechanism to select
  fd_set readfds;
  for (int i = 0; i < num_players; i++) {
    FD_SET(player_socketfd[i], &readfds);
  }
  int max_fd = *max_element(player_socketfd.begin(), player_socketfd.end());
  select(max_fd+1, &readfds, NULL, NULL, NULL);
  for (int i = 0; i < num_players; i++) {
    if (FD_ISSET(player_socketfd[i], &readfds)) {
      recv(player_socketfd[i], &p, sizeof(p), 0);      
      break;
    }
  }
  // print trace
  cout << "Trace of potato:" << endl;
  for (int i = 0; i < num_hops-1; i++) {
    cout << p.trace[i] << "," ;
  }
  cout << p.trace[num_hops-1] << endl;
  

  
  // end application
  for (int fd: player_socketfd) {
    close(fd);
  }
  close(socket_fd);
  return 0;
}
