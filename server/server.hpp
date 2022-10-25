#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Server
{
    public:

        Server();
        ~Server();

        //getters & setters
        void            set_server_port(int t_server_port);
        int             get_server_port() const;

        void            set_server_address();
        sockaddr_in     get_server_address() const;

        void            create_server_socket(); // < 0 is failure to establish server socket
        int             get_server_socket() const;

        void            set_server_message(std::string& t_message);
        std::string     get_server_message();


    private:
        int m_server_port;
        int m_server_socket;
        sockaddr_in m_server_address;

        std::vector<std::string> m_server_messages;

};

#endif