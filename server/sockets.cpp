#include "sockets.hpp"

Sockets::Sockets(int server_port)
{
    create_listening_sd();
    bind_and_listen();
    init_socket_set();
}

Sockets::~Sockets()
{
}

void Sockets::check(int ret, std::string error_message)
{
    if (ret < 0)
    {
        std::cout << error_message << std::endl;
        m_sockets_setup = false;
    }
}

bool Sockets::sockets_setup()
{
    return m_sockets_setup;
}

void Sockets::create_listening_sd()
{
    int on = 1;
    m_listening_sd = socket(AF_INET6, SOCK_STREAM, 0);

    check(setsockopt(m_listening_sd, SOL_SOCKET, SO_REUSEADDR,
                     (char *)&on, sizeof(on)),
          "setsockopt() failed");

    if (ioctl(m_listening_sd, FIONBIO, (char *)&on) < 0)
    {
        perror("ioctl() failed");
        close(m_listening_sd);
        m_sockets_setup = false;
    }
}

void Sockets::bind_and_listen()
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

void Sockets::init_socket_set()
{
    memset(m_socket_set, 0, sizeof(m_socket_set));
    m_socket_set[0].fd = m_listening_sd;
    m_socket_set[0].events = POLLIN;
    m_socket_count = 1;
}

void Sockets::poll_sockets()
{
    check(poll(m_socket_set, m_socket_count, TIMEOUT), "poll() failed");
}

bool Sockets::socket_event_manager()
{
    int current_size = m_socket_count;
    for (int i = 0; i < current_size; i++)
    {
        printf("Socket: %d .revents = %d\n", m_socket_set[i].fd, m_socket_set[i].revents);
        if (m_socket_set[i].revents == 0)
            continue;

        if (m_socket_set[i].revents == 17)
        {
            close_socket_connection(m_socket_set[i].fd);
            compress_socket_set();
            break;
        }

        if (m_socket_set[i].revents != POLLIN)
        {
            /////////////ISSUES HERE WITH ON CLIENT SENDING MESSAGE //////////////
            ////////////RANDOMLY REVENT IS 32 AND CAUSES ERROR BREAK/////////////
            /////////////////NEED SOLUTTION///////////////////////////////////////
            printf("Error! Socket: %d .revents = %d\n", m_socket_set[i].fd, m_socket_set[i].revents);
            // close_socket_connection(m_socket_set[i].fd);
            // compress_socket_set();
            // break;
        }

        // connections
        if (m_socket_set[i].fd == m_listening_sd)
        {
            std::cout << ">>Listening<<" << std::endl;
            accept_new_connections();
        }

        // inbound messages
        else
        {
            memset(m_buffer, 0, sizeof(m_buffer));
            printf("Socket: %d is requesting\n", m_socket_set[i].fd);

            if (receive_message(m_socket_set[i].fd) <= 0)
                break;

            m_buffer[m_buffer_length] = '\0';
            printf("%d bytes received - '%s'\n", m_buffer_length, m_buffer);

            if (send_message(m_socket_set[i].fd) < 0)
                break;

            if (m_close_connection)
                close_socket_connection(i);
        }
    }
    return false;
}

void Sockets::accept_new_connections()
{
    m_new_sd = accept(m_listening_sd, NULL, NULL);
    if (m_new_sd < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            perror("accept() failed");
            return;
        }
    }
    update_socket_set();
}

void Sockets::update_socket_set()
{
    printf("New incoming connection at socket: %d\n", m_new_sd);
    m_socket_set[m_socket_count].fd = m_new_sd;
    m_socket_set[m_socket_count].events = POLLIN;
    m_socket_count++;
}

int Sockets::receive_message(int socket_fd)
{
    m_buffer_length = recv(socket_fd, m_buffer, sizeof(m_buffer), 0);
    if (m_buffer_length < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            std::cout << "recv() failed" << std::endl;
            m_close_connection = true;
        }
    }
    std::cout << "m_buffer_length " << m_buffer_length << std::endl;
    if (m_buffer_length < 1)
    {
        std::cout << "Connection closed" << std::endl;
        m_close_connection = true;
    }
    return m_buffer_length;
}

int Sockets::send_message(int socket_fd)
{
    if (send(socket_fd, m_buffer, m_buffer_length, 0) < 0)
    {
        perror("send() failed");
        m_close_connection = true;
        return -1;
    }
    return m_buffer_length;
}

void Sockets::compress_socket_set()
{
    if (m_compress_socket_set)
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

void Sockets::close_socket_connection(int &socket)
{
    std::cout << "close_socket_connection()" << std::endl;
    close(socket);
    socket = -1;
    m_compress_socket_set = true;
}

void Sockets::close_sockets()
{
    for (int i = 0; i < m_socket_count; i++)
    {
        if (m_socket_set[i].fd >= 0)
            close(m_socket_set[i].fd);
    }
}