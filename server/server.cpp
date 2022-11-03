#include "server.hpp"

#define TIMEOUT 1000

Server::Server()
{
    std::cout << "Setting up listening socket...." << std::endl;
    check(create_listening_sd(), "Failed to create listening socket");
    bind_and_listen();
    init_socket_set();
    std::cout << "Server started:: Port::" << SERVER_PORT << " IP::" << LOCAL_IP << std::endl;
    std::cout << "Waiting for clients...." << std::endl;
}

Server::~Server()
{
    std::cout << "Closing all open sockets" << std::endl;
    close_sockets();
    std::cout << "Destroyed Server" << std::endl;
}

void Server::check(int ret, std::string error_message)
{
    if (ret < 0)
    {
        std::cout << error_message << std::endl;
        exit(-1);
    }
}

void Server::main_loop()
{
    while (m_exit_server == false)
    {
        check(poll(m_socket_set, m_socket_count, TIMEOUT), "poll() failed");

        socket_event_manager(); //managing connections and messages
    
        compress_socket_set(); //reorgs sockets when necessary
    }
}

int Server::create_listening_sd()
{
    int on = 1;
    m_listening_sd = socket(AF_INET6, SOCK_STREAM, 0);

    check(setsockopt(m_listening_sd, SOL_SOCKET, SO_REUSEADDR,
                    (char *)&on, sizeof(on)), "setsockopt() failed");

    if (ioctl(m_listening_sd, FIONBIO, (char *)&on) < 0)
    {
        perror("ioctl() failed");
        close(m_listening_sd);
        exit(-1);
    }

    return m_listening_sd;
}

void Server::bind_and_listen()
{
    memset(&m_server_address, 0, sizeof(m_server_address));
    m_server_address.sin6_family = AF_INET6;
    memcpy(&m_server_address.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    m_server_address.sin6_port = htons(SERVER_PORT);
    if (bind(m_listening_sd, (struct sockaddr *)&m_server_address, sizeof(m_server_address)) < 0)
    {
        perror("bind() failed");
        close(m_listening_sd);
        exit(-1);
    }

    if (listen(m_listening_sd, MAX_CONNECTIONS) < 0)
    {
        perror("m_listening() failed");
        close(m_listening_sd);
        exit(-1);
    }
}

void Server::init_socket_set()
{
    memset(m_socket_set, 0, sizeof(m_socket_set));
    m_socket_set[0].fd = m_listening_sd;
    m_socket_set[0].events = POLLIN;
    m_socket_count = 1;
}

////////////////////////////////////////////////////////////////
////////////Main server logic here!!!///////////////////////////
////////////////////////////////////////////////////////////////

void Server::socket_event_manager()
{
    int current_size = m_socket_count;
    for (int i = 0; i < current_size; i++)
    {
        if (m_socket_set[i].revents == 0)
            continue; //skip iteration

        if (m_socket_set[i].revents != POLLIN)
        {
            printf("Error! Socket: %d .revents = %d\n", m_socket_set[i].fd, m_socket_set[i].revents);
            m_exit_server = true;
            break;
        }

        //connections
        if (m_socket_set[i].fd == m_listening_sd)
        {
            std::cout << ">>Listening<<" << std::endl;
            accept_new_connections();
        }

        //inbound messages
        else
        {
            printf("Socket: %d is requesting\n", m_socket_set[i].fd);
         
            if(receive_message(m_socket_set[i].fd) <= 0)
                break;

            m_buffer[m_buffer_length] = '\0';
            printf("%d bytes received - '%s'\n", m_buffer_length, m_buffer);
           
            if(send_message(m_socket_set[i].fd) < 0)
                break;
    
            if (m_close_connection)
                close_socket_connection(m_socket_set[i].fd);
        } 
    }
    compress_socket_set();
}
////////////////////////////////////////////////////////////////

void Server::accept_new_connections()
{
    do
    {
        m_new_sd = accept(m_listening_sd, NULL, NULL);
        if (m_new_sd < 0)
        {
            if (errno != EWOULDBLOCK)
            {
                perror("accept() failed");
                m_exit_server = true;
            }
            break;
        }
        update_socket_set();
    } while (m_new_sd != -1);
}

void Server::update_socket_set()
{
    printf("New incoming connection at socket: %d\n", m_new_sd);
    m_socket_set[m_socket_count].fd = m_new_sd;
    m_socket_set[m_socket_count].events = POLLIN;
    m_socket_count++;
}

////////////////////////////////////////////////////////////////////

int Server::receive_message(int socket_fd)
{
    m_buffer_length = recv(socket_fd, m_buffer, sizeof(m_buffer), 0);
    if (m_buffer_length < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            perror("recv() failed");
            m_close_connection = true;
        }
    }
    if (m_buffer_length == 0)
    {
        std::cout << "Connection closed" << std::endl;
        m_close_connection = true;
    }
    return m_buffer_length;
}

int Server::send_message(int socket_fd)
{
    if (send(socket_fd, m_buffer, m_buffer_length, 0) < 0)
    {
        perror("send() failed");
        m_close_connection = true;
        return -1;
    }
    return m_buffer_length;
}

////////////////////////////////////////////////////////////////////////////

void Server::compress_socket_set()
{
    if(m_compress_socket_set)
    {
        for (int i = 0; i < m_socket_count; i++)
        {
            if (m_socket_set[i].fd == -1)
            {
                for (int j = i; j < m_socket_count; j++)
                    m_socket_set[j].fd = m_socket_set[j + 1].fd;

                i--;
                m_socket_count--;
            }
        }
    }
    m_compress_socket_set = false;
}

void Server::close_socket_connection(int socket)
{
    close(socket);
    socket = -1;
    m_compress_socket_set = true;
}

void Server::close_sockets()
{
    for (int i = 0; i < m_socket_count; i++)
    {
        if (m_socket_set[i].fd >= 0)
            close(m_socket_set[i].fd);
    }
}