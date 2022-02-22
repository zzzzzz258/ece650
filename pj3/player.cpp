#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "potato.h"
#include "socket_lib.h"

using namespace std;

int main(int argc, char * argv[]) {
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

  const char * hostname = argv[1];
  const char * port = argv[2];

  // get a socket connecting to ringmaster
  int socket_fd_ringmaster = build_client(hostname, port);
  int id = -1;
  int total_players = -1;

  // receive setup messages from ringmaster
  recv(socket_fd_ringmaster, &id, sizeof(id), 0);
  recv(socket_fd_ringmaster, &total_players, sizeof(total_players), 0);

  cout << "Connected as player " << id << " out of " << total_players << " total players"
       << endl;

  //char my_hostname[128];
  //gethostname(my_hostname, 128);

  // build a server socket
  //  int right_server_fd = build_server(right_server_port);
  int right_server_fd = build_server("");

  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  getsockname(right_server_fd, (struct sockaddr *)&addr, &len);
  // TODO: not checking return value
  int use_port = ntohs(addr.sin_port);
  char right_server_port[10] = {0};
  sprintf(right_server_port, "%d", use_port);

  // send server information to ringmaster
  //send(socket_fd_ringmaster, my_hostname, strlen(my_hostname), 0);  // hostname
  send(socket_fd_ringmaster,
       right_server_port,
       strlen(right_server_port),
       0);  // port number

  // receive left neighbor server info from ringmaster

  char left_player_hostname[128] = {0};
  char message[128] = {0};
  int lpp;
  char left_player_port[10] = {0};
  int n;
  //n = recv(socket_fd_ringmaster, left_player_hostname, sizeof(left_player_hostname), 0);
  n = recv(socket_fd_ringmaster, message, sizeof(message), 0);
  left_player_hostname[n] = 0;
  std::string msgs(message);
  size_t lpos = msgs.find('|');
  std::string l_hostname = msgs.substr(0, lpos);
  std::string l_port = msgs.substr(lpos+1, msgs.length() - 1 - lpos);
  cout << "Receive left neighbor hostname: " << l_hostname << endl;

  //n = recv(socket_fd_ringmaster, &lpp, sizeof(lpp), 0);
  //sprintf(left_player_port, "%d", lpp);
   cout << "Receive port number is " << l_port << endl;

  strcpy(left_player_hostname,l_hostname.c_str());
  strcpy(left_player_port,l_port.c_str());
   
   
  //cout << left_player_hostname << endl;
  //cout << left_player_port << endl;

  int left_player_fd, right_player_fd;
  if (id == 1) {
    // connect to left player
    left_player_fd = build_client(left_player_hostname, left_player_port);
    string s;
    right_player_fd = server_accept(right_server_fd, s);
  }
  else {
    string s;
    right_player_fd = server_accept(right_server_fd, s);
    left_player_fd = build_client(left_player_hostname, left_player_port);
  }

  // send msg to ringmaster to indicate that all connections are successfuly established
  int msg = 1;
  send(socket_fd_ringmaster, &msg, sizeof(msg), 0);

  // set selector
  fd_set readfds;
  int maxfd = max(right_player_fd, max(socket_fd_ringmaster, left_player_fd));
  potato p;
  srand((unsigned int)time(NULL) + total_players);

  while (true) {
    FD_ZERO(&readfds);

    FD_SET(socket_fd_ringmaster, &readfds);
    FD_SET(left_player_fd, &readfds);
    FD_SET(right_player_fd, &readfds);

    select(maxfd + 1, &readfds, NULL, NULL, NULL);
    bool to_master = false;
    if (FD_ISSET(socket_fd_ringmaster, &readfds)) {
      int n = recv(socket_fd_ringmaster, &p, sizeof(p), 0);
      if (p.left_hops == 0) {
        break;
      }
      else if (p.left_hops == 1) {
        // only this one hop
        cout << "I'm it" << endl;
        to_master = true;
      }
    }
    else if (FD_ISSET(left_player_fd, &readfds) || FD_ISSET(right_player_fd, &readfds)) {
      int source_player =
          FD_ISSET(left_player_fd, &readfds) ? left_player_fd : right_player_fd;
      recv(source_player, &p, sizeof(p), 0);
      if (p.left_hops == 1) {
        // game ends, send information to ringmaster
        cout << "I'm it" << endl;
        to_master = true;
      }
    }
    else {
      continue;
    }

    // update potato and pass it to next if received from other
    p.trace[p.used_hops] = id;
    p.used_hops++;
    p.left_hops--;
    if (to_master) {
      send(socket_fd_ringmaster, &p, sizeof(p), 0);
      continue;
    }
    int random = rand() % 2;
    if (random % 2 == 0) {
      send(left_player_fd, &p, sizeof(p), 0);
      cout << "Sending potato to " << (id - 1 + total_players) % total_players << endl;
    }
    else {
      send(right_player_fd, &p, sizeof(p), 0);
      cout << "Sending potato to " << (id + 1) % total_players << endl;
    }
  }

  // game ends, close sockets
  n = recv(socket_fd_ringmaster, NULL, 0, 0);
  if (n == 0) {
    close(socket_fd_ringmaster);
    close(right_server_fd);
    close(left_player_fd);
    close(right_player_fd);
  }

  return 0;
}
