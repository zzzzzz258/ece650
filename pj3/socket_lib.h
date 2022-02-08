#include <stdbool.h>
#include <string>

int build_server(const char * port);
int server_accept(int server_socket_fd);
bool argv_number_check(char *argv[], int index, const std::string arg_name);
int build_client(const char * _hostname, const char * _port);
