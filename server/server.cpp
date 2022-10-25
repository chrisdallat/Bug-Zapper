#include "server.hpp"

Server::Server()
{
    std::cout << "Constructed Server" << std::endl;
}

Server::~Server()
{
    std::cout << "Destroyed Server" << std::endl;
}

void Server::set_server_address()
{
    // bzero((char *)&m_server_address, sizeof(m_server_address));
    m_server_address = {0}; // initialize to zero with "{}" or "= {0}"
    m_server_address.sin_family = AF_INET;
    m_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    m_server_address.sin_port = htons(m_server_port);
}

sockaddr_in Server::get_server_address() const
{
    return m_server_address;
}

void Server::create_server_socket()
{
    m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
}

int Server::get_server_socket() const
{
    return m_server_socket;
}

void Server::set_server_port(int t_server_port)
{
    m_server_port = t_server_port;
}

int Server::get_server_port() const
{
    if (m_server_port)
        return m_server_port;
    else
        return -1;
}

void Server::set_server_message(std::string &t_message)
{
    m_server_messages.push_back(t_message);
}

std::string Server::get_server_message()
{
    if (!m_server_messages.empty())
    {
        std::string message = m_server_messages.back();
        m_server_messages.pop_back();
        return message;
    }
    else
        return "error: no messages in m_server_messages";
}
