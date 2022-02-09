#include <stdbool.h>
#include <string>

struct _simple_addr {
  char hostname[128];
  char port[6];
};
typedef struct _simple_addr simple_addr;

void connect_to_server(int socket_fd,
                       struct addrinfo * server_info,
                       const char * hostname,
                       const char * port);
int build_server(const char * port);
int server_accept(int server_socket_fd);
bool argv_number_check(char *argv[], int index, const std::string arg_name);
int build_client(const char * _hostname, const char * _port);
int get_port(int socket_fd);
