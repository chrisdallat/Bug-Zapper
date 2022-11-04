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

#include "sockets.hpp"

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

    void                main_loop();

    void                socket_event_manager(); //manage sockets polled events


    // int                 receive_message(int socket_fd);
    // int                 send_message(int socket_fd);
    

    //-------------------public variables------------------------------//

private:
    Sockets m_sockets;
    
    char m_buffer[MAX_BUFFER_SIZE];
    int m_buffer_length;

    //-------------------bools---------------------------------------//
    bool m_exit_server;

};

#endif