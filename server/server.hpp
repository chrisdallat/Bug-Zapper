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

#define MAX_BUFFER_SIZE 1024
#define POLL_TIMEOUT 3000
#define MAX_CONNECTIONS 10
#define SERVER_PORT "52000"

class Server
{
    public:

        Server();
        ~Server();

        void            main_loop();

        //getters & setters
        void            set_server_port(int t_server_port);
        int             get_server_port() const;

        void            set_server_address();
        sockaddr_in     get_server_address() const;

        void            create_server_listening_socket();
        int             create_socket(int socket_fd);
        int             get_server_listening_socket() const;

        void            init_socket_set();

        void            set_server_message(std::string& t_message);
        std::string     get_server_message();

        void            check(int res);


        //-------------------public variables------------------------------//
        
    private:

        //-------------------bools----------------------------------------//

        bool m_exit;

        //-------------------server data------------------------------//

        int m_server_port;
        int m_server_listening_socket;
        sockaddr_in m_server_address;
        pollfd m_socket_set[MAX_CONNECTIONS];
        int m_socket_count;
        int m_current_socket; //current in loops

        //-------------------messages data ------------------------------//

        std::vector<std::string> m_server_messages;

};

#endif