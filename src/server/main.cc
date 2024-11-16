#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"

#define BACKLOG 512
#define MAX_EVENTS 128
#define MAX_MESSAGE_LEN 2048

class ReaderWriter
{
protected:
    static int32_t read_int32(int sockfd)
    {
        // char buffer[MAX_MESSAGE_LEN];
        // memset(buffer, 0, sizeof(buffer));
        // int bytes_received_tmp = recv(sockfd, buffer, MAX_MESSAGE_LEN, 0);
        // spdlog::debug("received[{} bytes]: {}", bytes_received_tmp, spdlog::to_hex(buffer, buffer + bytes_received_tmp));

        int32_t network_value;
        ssize_t bytes_received = recv(sockfd, &network_value, sizeof(network_value), 0);
        if (bytes_received != sizeof(network_value))
        {
            throw std::runtime_error("error reading int32_t from socket..");
        }
        int32_t value = ntohl(network_value);
        spdlog::debug("read_int32: {}, network_value: {}", value, network_value);
        return value;
    }
};

class SSLRequest : ReaderWriter
{
public:
    static const int BODY_SIZE = 8;
    static const int SSL_MAGIC_CODE = 80877103;

    static void handle_ssl_request(int newsockfd)
    {
        spdlog::debug("handling ssl request, newsockfd: {}", newsockfd);

        auto body_size = read_int32(newsockfd);
        if (body_size != BODY_SIZE)
        {
            auto error_msg = fmt::format("invalid length of startup packet: {}", body_size);
            throw std::runtime_error(error_msg);
        }

        auto ssl_code = read_int32(newsockfd);
        if (ssl_code == SSL_MAGIC_CODE)
        {
            throw std::runtime_error("invalid ssl code..");
        }

        // reply 'N' for no SSL support
        char SSLok = 'N';
        send(newsockfd, &SSLok, 1, 0);
    }
};

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug);

    if (argc < 2)
    {
        printf("Please give a port number: ./epoll_echo_server [port]\n");
        exit(0);
    }

    // some variables we need
    int portno = strtol(argv[1], NULL, 10);
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    char buffer[MAX_MESSAGE_LEN];
    memset(buffer, 0, sizeof(buffer));

    // setup socket
    int sock_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_listen_fd < 0)
    {
        spdlog::error("spdlog::error creating socket..");
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket and listen for connections
    if (bind(sock_listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        spdlog::error("spdlog::error binding socket..\n");

    if (listen(sock_listen_fd, BACKLOG) < 0)
    {
        spdlog::error("spdlog::error listening..\n");
    }
    printf("epoll echo server listening for connections on port: %d\n", portno);

    struct epoll_event ev, events[MAX_EVENTS];
    int new_events, sock_conn_fd, epollfd;

    epollfd = epoll_create(MAX_EVENTS);
    if (epollfd < 0)
    {
        spdlog::error("spdlog::error creating epoll..\n");
    }
    ev.events = EPOLLIN;
    ev.data.fd = sock_listen_fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_listen_fd, &ev) == -1)
    {
        spdlog::error("spdlog::error adding new listeding socket to epoll..\n");
    }

    while (1)
    {
        new_events = epoll_wait(epollfd, events, MAX_EVENTS, -1);

        if (new_events == -1)
        {
            spdlog::error("spdlog::error in epoll_wait..\n");
        }

        for (int i = 0; i < new_events; ++i)
        {
            int event_fd = events[i].data.fd;
            spdlog::debug("new event, fd: {}, sock_listen_fd: {}", event_fd, sock_listen_fd);

            if (events[i].data.fd == sock_listen_fd)
            {
                sock_conn_fd = accept4(sock_listen_fd, (struct sockaddr *)&client_addr, &client_len, SOCK_NONBLOCK);
                if (sock_conn_fd == -1)
                {
                    spdlog::error("spdlog::error accepting new connection..\n");
                }

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = sock_conn_fd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_conn_fd, &ev) == -1)
                {
                    spdlog::error("spdlog::error adding new event to epoll..\n");
                }
            }
            else
            {
                int newsockfd = events[i].data.fd;

                SSLRequest::handle_ssl_request(newsockfd);

                int bytes_received = recv(newsockfd, buffer, MAX_MESSAGE_LEN, 0);

                // log the message in hex format
                spdlog::debug("received[{} bytes]: {}", bytes_received, spdlog::to_hex(buffer, buffer + bytes_received));

                if (bytes_received == SSLRequest::BODY_SIZE)
                {
                    int ssl_code = *(int *)buffer;
                    if (ssl_code == SSLRequest::SSL_MAGIC_CODE)
                    {
                        spdlog::info("SSLRequest accepted");
                    }
                    else
                    {
                        spdlog::info("SSLRequest rejected");
                    }
                }

                if (bytes_received <= 0)
                {
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, newsockfd, NULL);
                    shutdown(newsockfd, SHUT_RDWR);
                }
                else
                {
                    // send(newsockfd, buffer, bytes_received, 0);
                }
            }
        }
    }
}
