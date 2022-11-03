#ifndef SERVER_HPP
#define SERVER_HPP

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

#define MAX_BUFFER_SIZE 1024
#define POLL_TIMEOUT 3000
#define MAX_CONNECTIONS 10
#define SERVER_PORT 52000

class Server
{
public:
    Server();
    ~Server();

    int                 create_listening_sd();
    void                bind_and_listen();

    void                init_socket_set();
    void                compress_socket_set();

    void                check(int ret, std::string error_message);

    void                main_loop();

    void                close_sockets();    

    //-------------------public variables------------------------------//

private:
    
    int m_listening_sd; 
    int m_new_sd;

    sockaddr_in6 m_server_address;

    pollfd m_socket_set[MAX_CONNECTIONS];
    int m_socket_count;
    
    char m_buffer[MAX_BUFFER_SIZE];

    //-------------------bools---------------------------------------//
    bool m_exit_server;
    bool m_compress_socket_set;
    bool m_close_connection;

};

#endif