#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int build_server(const char * select_port) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;
  const char *port     = select_port;

  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if

  
  freeaddrinfo(host_info_list);

  return socket_fd;
}

void connect_to_server(int socket_fd,
                       struct addrinfo * server_info,
                       const char * hostname,
                       const char * port) {
  int status = connect(socket_fd, server_info->ai_addr, server_info->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if
}

/**
 * Build a client socket connecting to given host
 * @return socket_fd is the created socket_fd for this connection as client
 */
int build_client(const char * _hostname, const char * _port) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = _hostname;
  const char *port     = _port;
  
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if

  connect_to_server(socket_fd, host_info_list, hostname, port);
  
  freeaddrinfo(host_info_list);
  return socket_fd;
}

/**
 * server socket behavior
 * accpet a connection with error check, return the new socket with connection
 */
int server_accept(int server_socket_fd) {
  int client_connection_fd;
  struct sockaddr socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  client_connection_fd = accept(server_socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (client_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    exit(EXIT_FAILURE);
  } //if
  //struct sockaddr_in * addr = (struct sockaddr_in *)&socket_addr;
  //*ip = inet
  return client_connection_fd;
}

int get_port(int socket_fd) {
  // only for IPV4
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  if (getsockname(socket_fd, (struct sockaddr *)&sin, &len) != 0) {
    cout << "Cannot get sockname" << endl;
    exit(EXIT_FAILURE);
  }
  return ntohs(sin.sin_port);
}

bool is_number(char* str) {
  char * pos = str;
  while (*pos != 0) {
    if (*pos < '0' || *pos > '9') {
      return false;
    }
    pos++;
  }
  return true;
}

bool argv_number_check(char *argv[], int index, const string arg_name) {
  if (!is_number(argv[index])) {
    cout << "Syntax: ringmaster <port_num> <num_players> <num_hops>" << endl;
    cout << arg_name << " should be a number" << endl;
    return true;
  }
  return false;
}
