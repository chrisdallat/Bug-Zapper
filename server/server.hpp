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
#define LOCAL_IP "127.0.0.1"

class Server
{
public:
    Server();
    ~Server();
    
    void                check(int ret, std::string error_message);
    void                main_loop();

    int                 create_listening_sd();
    void                bind_and_listen();

    void                init_socket_set();
    void                socket_event_manager(); //manage sockets polled events
    void                accept_new_connections();
    void                update_socket_set();

    int                 receive_message(int socket_fd);
    int                 send_message(int socket_fd);
    
    void                close_socket_connection(int socket);
    void                compress_socket_set();
    void                close_sockets();    

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
    bool m_exit_server;
    bool m_compress_socket_set;
    bool m_close_connection;

};

#endif