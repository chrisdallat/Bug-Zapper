#ifndef SOCKETS_HPP
#define SOCKETS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#include <sys/ioctl.h>

#define SERVER_PORT 52000
#define MAX_BUFFER_SIZE 1024
#define MAX_CONNECTIONS 10
#define TIMEOUT 3000

class Sockets
{
public:
    Sockets(int server_port);
    ~Sockets();

    void                check(int ret, std::string error_message);      //
    bool                sockets_setup();

    void                create_listening_sd();                          //
    void                bind_and_listen();                              //

    void                init_socket_set();                              //
    void                poll_sockets();
    bool                socket_event_manager(); 
    void                accept_new_connections();                       //
    void                update_socket_set();                            //

    int                 receive_message(int socket_fd);
    int                 send_message(int socket_fd);
    
    void                close_socket_connection(int &socket);               
    void                compress_socket_set();                             //
    void                close_sockets();                                   //

    //-------------------public variables------------------------------//

private:
    
    int m_listening_sd; 
    int m_new_sd;

    struct sockaddr_in6 m_server_address;

    pollfd m_socket_set[MAX_CONNECTIONS];
    int m_socket_count;

    char m_buffer[MAX_BUFFER_SIZE];
    int m_buffer_length;

    //-------------------bools---------------------------------------//
    bool m_sockets_setup;
    bool m_compress_socket_set;
    bool m_close_connection;

};

#endif