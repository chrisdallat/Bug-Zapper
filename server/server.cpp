#include "server.hpp"

#define TIMEOUT 1000

Server::Server()
{
    check(create_listening_sd(), "Failed to create listening socket");
}

Server::~Server()
{
    std::cout << "Destroyed Server" << std::endl;
}

void Server::init_socket_set()
{
    memset(m_socket_set, 0, sizeof(m_socket_set));
    m_socket_set[0].fd = m_listening_sd;
    m_socket_set[0].events = POLLIN;
    m_socket_count = 1;
}

void Server::check(int ret, std::string error_message)
{
    if (ret < 0)
    {
        std::cout << error_message << std::endl;
        exit(-1);
    }
}

int Server::create_listening_sd()
{
    int on = 1;

    m_listening_sd = socket(AF_INET6, SOCK_STREAM, 0);

    check(setsockopt(m_listening_sd, SOL_SOCKET, SO_REUSEADDR,
                    (char *)&on, sizeof(on)), "setsockopt() failed");
    /*************************************************************/
    /* Set socket to be nonblocking. All of the sockets for      */
    /* the incoming connections will also be nonblocking since   */
    /* they will inherit that state from the m_listeninging socket.*/
    /*************************************************************/
    if (ioctl(m_listening_sd, FIONBIO, (char *)&on) < 0)
    {
        perror("ioctl() failed");
        close(m_listening_sd);
        exit(-1);
    }

    return m_listening_sd;
}

void Server::main_loop()
{
    int len, ret, on = 1;
    struct sockaddr_in6 addr;
    int timeout;
    int current_size = 0, i, j;

    /*************************************************************/
    /* Bind the socket                                           */
    /*************************************************************/
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    addr.sin6_port = htons(SERVER_PORT);
    ret = bind(m_listening_sd,
              (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        perror("bind() failed");
        close(m_listening_sd);
        exit(-1);
    }

    /*************************************************************/
    /* Set the m_listening back log                                   */
    /*************************************************************/
    ret = listen(m_listening_sd, 32);
    if (ret < 0)
    {
        perror("m_listening() failed");
        close(m_listening_sd);
        exit(-1);
    }

    /*************************************************************/
    /* Initialize the pollfd structure                           */
    /* Set up the initial m_listeninging socket                  */
    /*************************************************************/
    init_socket_set();

    /*************************************************************/
    /* Loop waiting for incoming connects or for incoming data   */
    /* on any of the connected sockets.                          */
    /*************************************************************/
    do
    {
        /***********************************************************/
        /* Call poll() and wait 3 minutes for it to complete.      */
        /***********************************************************/
        printf("Waiting on poll()...\n");
        check(poll(m_socket_set, m_socket_count, TIMEOUT), "poll() failed");

        /***********************************************************/
        /* One or more descriptors are readable.  Need to          */
        /* determine which ones they are.                          */
        /***********************************************************/
        current_size = m_socket_count;
        for (i = 0; i < current_size; i++)
        {
            /*********************************************************/
            /* Loop through to find the descriptors that returned    */
            /* POLLIN and determine whether it's the m_listeninging       */
            /* or the active connection.                             */
            /*********************************************************/
            if (m_socket_set[i].revents == 0)
                continue;

            /*********************************************************/
            /* If revents is not POLLIN, it's an unexpected result,  */
            /* log and end the server.                               */
            /*********************************************************/
            if (m_socket_set[i].revents != POLLIN)
            {
                printf("  Error! revents = %d\n", m_socket_set[i].revents);
                m_exit_server = true;
                break;
            }
            if (m_socket_set[i].fd == m_listening_sd)
            {
                /*******************************************************/
                /* m_listeninging descriptor is readable.                   */
                /*******************************************************/
                printf("  m_listeninging socket is readable\n");

                /*******************************************************/
                /* Accept all incoming connections that are            */
                /* queued up on the m_listeninging socket before we         */
                /* loop back and call poll again.                      */
                /*******************************************************/
                do
                {
                    /*****************************************************/
                    /* Accept each incoming connection. If               */
                    /* accept fails with EWOULDBLOCK, then we            */
                    /* have accepted all of them. Any other              */
                    /* failure on accept will cause us to end the        */
                    /* server.                                           */
                    /*****************************************************/
                    m_new_sd = accept(m_listening_sd, NULL, NULL);
                    if (m_new_sd < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("  accept() failed");
                            m_exit_server = true;
                        }
                        break;
                    }

                    /*****************************************************/
                    /* Add the new incoming connection to the            */
                    /* pollfd structure                                  */
                    /*****************************************************/
                    printf("  New incoming connection - %d\n", m_new_sd);
                    m_socket_set[m_socket_count].fd = m_new_sd;
                    m_socket_set[m_socket_count].events = POLLIN;
                    m_socket_count++;

                    /*****************************************************/
                    /* Loop back up and accept another incoming          */
                    /* connection                                        */
                    /*****************************************************/
                } while (m_new_sd != -1);
            }

            /*********************************************************/
            /* This is not the m_listeninging socket, therefore an        */
            /* existing connection must be readable                  */
            /*********************************************************/

            else
            {
                printf("  Descriptor %d is readable\n", m_socket_set[i].fd);
                m_close_connection = false;
                /*******************************************************/
                /* Receive all incoming data on this socket            */
                /* before we loop back and call poll again.            */
                /*******************************************************/

                do
                {
                    /*****************************************************/
                    /* Receive data on this connection until the         */
                    /* recv fails with EWOULDBLOCK. If any other         */
                    /* failure occurs, we will close the                 */
                    /* connection.                                       */
                    /*****************************************************/
                    ret = recv(m_socket_set[i].fd, m_buffer, sizeof(m_buffer), 0);
                    if (ret < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("  recv() failed");
                            m_close_connection = true;
                        }
                        break;
                    }

                    /*****************************************************/
                    /* Check to see if the connection has been           */
                    /* closed by the client                              */
                    /*****************************************************/
                    if (ret == 0)
                    {
                        printf("  Connection closed\n");
                        m_close_connection = true;
                        break;
                    }

                    /*****************************************************/
                    /* Data was received                                 */
                    /*****************************************************/
                    len = ret;
                    printf("  %d bytes received\n", len);

                    /*****************************************************/
                    /* Echo the data back to the client                  */
                    /*****************************************************/
                    ret = send(m_socket_set[i].fd, m_buffer, len, 0);
                    if (ret < 0)
                    {
                        perror("  send() failed");
                        m_close_connection = true;
                        break;
                    }

                } while (true);

                /*******************************************************/
                /* If the close_conn flag was turned on, we need       */
                /* to clean up this active connection. This            */
                /* clean up process includes removing the              */
                /* descriptor.                                         */
                /*******************************************************/
                if (m_close_connection)
                {
                    close(m_socket_set[i].fd);
                    m_socket_set[i].fd = -1;
                    m_compress_socket_set = true;
                }

            } /* End of existing connection is readable             */
        }     /* End of loop through pollable descriptors              */

        /***********************************************************/
        /* If the m_compress_socket_set flag was turned on, we need       */
        /* to squeeze together the array and decrement the number  */
        /* of file descriptors. We do not need to move back the    */
        /* events and revents fields because the events will always*/
        /* be POLLIN in this case, and revents is output.          */
        /***********************************************************/
        if (m_compress_socket_set)
        {
            compress_socket_set();
        }

    } while (m_exit_server == false); /* End of serving running.    */

    /*************************************************************/
    /* Clean up all of the sockets that are open                 */
    /*************************************************************/
    close_sockets();
}

void Server::compress_socket_set()
{
    m_compress_socket_set = false;
    for (int i = 0; i < m_socket_count; i++)
    {
        if (m_socket_set[i].fd == -1)
        {
            for (int j = i; j < m_socket_count; j++)
            {
                m_socket_set[j].fd = m_socket_set[j + 1].fd;
            }
            i--;
            m_socket_count--;
        }
    }
}

void Server::close_sockets()
{
    for (int i = 0; i < m_socket_count; i++)
    {
        if (m_socket_set[i].fd >= 0)
            close(m_socket_set[i].fd);
    }
}