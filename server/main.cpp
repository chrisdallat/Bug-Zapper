#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

#include "server.hpp"

// Server side
int main()
{
    Server server;
    char msg[100];

    if (server.get_server_listening_socket() < 0)
    {
        std::cerr << "Error establishing the server socket" << std::endl;
        exit(0);
    }

    sockaddr_in server_addr = server.get_server_address();
    int bind_status = bind(server.get_server_listening_socket(), (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_status < 0)
    {
        std::cerr << "Error binding socket to local address" << std::endl;
        exit(0);
    }
    std::cout << "Waiting for a client to connect..." << std::endl;
    //set socket for listening;
    if(listen(server.get_server_listening_socket(), MAX_CONNECTIONS) < 0)
    {
        std::cerr << "Error";
        return -1;
    }
    setsockopt(server.get_server_listening_socket(), SOL_SOCKET,  SO_REUSEADDR, 0, 1);
        
    server.init_socket_set();

    server.main_loop();

    std::cout << "CLOSING SERVER" << std::endl;

    close(server.get_server_listening_socket());

    std::cout << "Connection closed..." << std::endl;
    return 0;
}
