#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <string>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>

#define SERVER_IP "127.0.0.1"



// Client side
int main(int argc, char *argv[])
{

    std::string server_ip = SERVER_IP;
    int port = 52000;
    // create a message buffer
    char msg[1500];
    // setup a socket and connection tools
    struct hostent *host = gethostbyname(server_ip.c_str());
    sockaddr_in send_socket_addr;
    bzero((char *)&send_socket_addr, sizeof(send_socket_addr));
    send_socket_addr.sin_family = AF_INET;
    send_socket_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    send_socket_addr.sin_port = htons(port);
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    // try to connect...
    int status = connect(client_socket, (sockaddr *)&send_socket_addr, sizeof(send_socket_addr));

    if (status < 0)
    {
        std::cout << "Error connecting to socket!" << std::endl;
        return -1;
    }
    std::cout << "Connected to the server!" << std::endl;
    std::cout << "client socket: " << client_socket << " status: " << status << std::endl;
    int bytes_read, bytes_written = 0;

    while (1)
    {
        std::cout << ">";
        std::string data;
        getline(std::cin, data);
        memset(&msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, data.c_str());
        if(data == "exit")
        {
            send(client_socket, (char*)&msg, strlen(msg), 0);
            break;
        }
        bytes_written += send(client_socket, (char*)&msg, strlen(msg), 0);
        std::cout << "Awaiting server response..." << std::endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        bytes_read += recv(client_socket, (char*)&msg, sizeof(msg), 0);
        if(!strcmp(msg, "exit"))
        {
            std::cout << "Server has quit the session" << std::endl;
            break;
        }
        std::cout << "Server: " << msg << std::endl;
    }

    close(client_socket);
    std::cout << "********Session********" << std::endl;
    std::cout << "Bytes written: " << bytes_written << " Bytes read: " << bytes_read << std::endl;
    std::cout << "Connection closed" << std::endl;
    return 0;
}

