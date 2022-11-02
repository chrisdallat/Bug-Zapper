#include "server.hpp"

Server::Server()
{
    std::cout << "Constructed Server" << std::endl;
    set_server_port(atoi(SERVER_PORT));
    set_server_address();
    create_server_listening_socket();
}

Server::~Server()
{
    std::cout << "Destroyed Server" << std::endl;
}

void Server::main_loop()
{
    while(!m_exit)
    {
        check(poll(m_socket_set, m_socket_count, 0));
        
        std::cout << "socket count: " << m_socket_count << std::endl;

        for (int i = 0; i < m_socket_count; i++)
		{
            m_current_socket = m_socket_set[i].fd;
            std::cout << "current socket: " << m_current_socket << std::endl;
			// Is it an inbound communication?
			if (m_current_socket == m_server_listening_socket)
			{
				// Accept a new connection
				int client_socket = accept(m_server_listening_socket, nullptr, nullptr);
                // std::cout << "client_socket = " << client_socket << std::endl;
                // std::cout << "m_current_socket = " << m_server_listening_socket << std::endl;
				// Add the new connection to the list of cnnected clients
                m_socket_set[m_socket_count].fd = client_socket;
                m_socket_set[m_socket_count].events = POLLIN;
                m_socket_count++;
				// Send a welcome message to the connected client
				std::string server_connection = "Connected to the Server\n";
				send(client_socket, server_connection.c_str(), server_connection.size() + 1, 0);
			}
			else // It's an inbound message
			{
				char buffer[MAX_BUFFER_SIZE];
				memset(buffer, 0, MAX_BUFFER_SIZE - 1);
				
				int bytes_recv = recv(m_socket_set[i].fd, buffer, MAX_BUFFER_SIZE - 1, 0);
				if (bytes_recv <= 0) // Drop the client
                {
					close(m_socket_set[i].fd);
                    m_socket_set[i].fd = -1;
                    m_socket_set[i].events = 0; 
                    m_socket_count--;
                }
				else
				{
					// TEMP::Check to see if it's a command. \killall kills the server
					if (buffer[0] == 'q')
                    {
                        m_exit = true;
                        break;
                    };
					std::cout << buffer << std::endl;
				}
			}
		}
    }
}

void Server::check(int res)
{
    if (res < 0)
    {
        perror("  poll() failed");
        m_exit = true;
    }
    return;
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

void Server::create_server_listening_socket()
{
    m_server_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
}

void Server::init_socket_set()
{
    m_socket_set[0].fd = m_server_listening_socket;
    m_socket_set[0].events = POLLIN;
    m_socket_count = 1;
    for (std::size_t i = 1; i < MAX_CONNECTIONS; i++) //initialise rest to -1
        m_socket_set[i].fd = -1;
}

int Server::create_socket(int socket_fd)
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int Server::get_server_listening_socket() const
{
    return m_server_listening_socket;
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
