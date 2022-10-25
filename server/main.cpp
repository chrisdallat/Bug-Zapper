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

    server.set_server_port(atoi("8080"));
    server.set_server_address();
    server.create_server_socket();

    if (server.get_server_socket() < 0)
    {
        std::cerr << "Error establishing the server socket" << std::endl;
        exit(0);
    }

    sockaddr_in server_addr = server.get_server_address();
    int bind_status = bind(server.get_server_socket(), (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bind_status < 0)
    {
        std::cerr << "Error binding socket to local address" << std::endl;
        exit(0);
    }
    std::cout << "Waiting for a client to connect..." << std::endl;
    // listen for up to 5 requests at a time
    listen(server.get_server_socket(), 5);
    // receive a request from client using accept
    // we need a new address to connect with the client
    sockaddr_in new_socket_addr;
    socklen_t new_socket_addr_size = sizeof(new_socket_addr);
    // accept, create a new socket descriptor to
    // handle the new connection with client
    int new_socket = accept(server.get_server_socket(), (sockaddr *)&new_socket_addr, &new_socket_addr_size);
    if (new_socket < 0)
    {
        std::cerr << "Error accepting request from client!" << std::endl;
        exit(1);
    }
    std::cout << "Connected with client!" << std::endl;

    // also keep track of the amount of data sent as well
    int bytes_read, bytes_written = 0;
    while(true){
    // receive a message from the client (listen)
    std::cout << "Awaiting client response..." << std::endl;
    memset(&msg, 0, sizeof(msg));//clear the buffer
    bytes_read += recv(new_socket, (char*)&msg, sizeof(msg), 0);
    if(!strcmp(msg, "exit"))
    {
        std::cout << "Client has quit the session" << std::endl;
        break;
    }
    std::cout << "Client: " << msg << std::endl;
    std::cout << ">";
    std::string data;
    getline(std::cin, data);
    memset(&msg, 0, sizeof(msg)); //clear the buffer
    strcpy(msg, data.c_str());
    if(data == "exit")
    {
        //send to the client that server has closed the connection
        send(new_socket, (char*)&msg, strlen(msg), 0);
        break;
    }
    //send the message to client
    bytes_written += send(new_socket, (char*)&msg, strlen(msg), 0);
    }


    close(new_socket);
    close(server.get_server_socket());
    std::cout << "********Session********" << std::endl;
    std::cout << "Bytes written: " << bytes_written << " Bytes read: " << bytes_read << std::endl;
    std::cout << "Connection closed..." << std::endl;
    return 0;
}
