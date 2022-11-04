#include "server.hpp"
#include "sockets.hpp"

Server::Server() : m_sockets(52000) 
{
    
    std::cout << "Setting up listening socket...." << std::endl;
    std::cout << "Server started:: Port::" << SERVER_PORT << " IP::" << LOCAL_IP << std::endl;
    std::cout << "Waiting for clients...." << std::endl;
}

Server::~Server()
{
    std::cout << "Closing all open sockets" << std::endl;
    m_sockets.close_sockets();
    std::cout << "Destroyed Server" << std::endl;
}

void Server::main_loop()
{
    while (m_exit_server == false)
    {
        m_sockets.poll_sockets();

        m_exit_server = m_sockets.socket_event_manager(); 
    
        // m_sockets.compress_socket_set();
    }
}

////////////////////////////////////////////////////////////////////

// int Server::receive_message(int socket_fd)
// {
//     m_buffer_length = recv(socket_fd, m_buffer, sizeof(m_buffer), 0);
//     if (m_buffer_length < 0)
//     {
//         if (errno != EWOULDBLOCK)
//         {
//             perror("recv() failed");
//             m_close_connection = true;
//         }
//     }
//     if (m_buffer_length == 0)
//     {
//         std::cout << "Connection closed" << std::endl;
//         m_close_connection = true;
//     }
//     return m_buffer_length;
// }

// int Server::send_message(int socket_fd)
// {
//     if (send(socket_fd, m_buffer, m_buffer_length, 0) < 0)
//     {
//         perror("send() failed");
//         m_close_connection = true;
//         return -1;
//     }
//     return m_buffer_length;
// }