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
#include <unordered_map>

// Must: define SPDLOG_ACTIVE_LEVEL before `#include "spdlog/spdlog.h"`
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include <mutex>

#define BACKLOG 512
#define MAX_EVENTS 128
#define MAX_MESSAGE_LEN 2048

class SocketsManager;

class ReaderWriter
{
protected:
    static int32_t read_int32(int sockfd)
    {
        int32_t network_value;
        ssize_t bytes_received = recv(sockfd, &network_value, sizeof(network_value), 0);
        if (bytes_received != sizeof(network_value))
        {
            throw std::runtime_error("error reading int32_t from socket..");
        }
        int32_t value = ntohl(network_value);
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
        SPDLOG_DEBUG("handling ssl request, newsockfd: {}", newsockfd);

        auto body_size = read_int32(newsockfd);
        if (body_size != BODY_SIZE)
        {
            auto error_msg = fmt::format("invalid length of startup packet: {}", body_size);
            throw std::runtime_error(error_msg);
        }

        auto ssl_code = read_int32(newsockfd);
        if (ssl_code != SSL_MAGIC_CODE)
        {
            auto error_msg = fmt::format("invalid ssl code: {}", ssl_code);
            throw std::runtime_error(error_msg);
        }

        // reply 'N' for no SSL support
        char SSLok = 'N';
        send(newsockfd, &SSLok, 1, 0);
        SocketsManager::set_socket_state(newsockfd, SocketsManager::SocketState::NoSSLAcknowledged);
    }
};

// template <>
// class fmt::formatter<SocketsManager::SocketState>
// {
// public:
//     constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
//     template <typename Context>
//     constexpr auto format(SocketsManager::SocketState const &foo, Context &ctx) const
//     {
//         // return format_to(ctx.out(), "({}, {})", foo.a, foo.b); // --== KEY LINE ==--
//         switch (state)
//         {
//         case SocketState::StartUp:
//             return "StartUp";
//         case SocketState::NoSSLAcknowledged:
//             return "NoSSLAcknowledged";
//         default:
//             return "Unknown";
//         }
//     }
// };

class SocketsManager
{
public:
    enum class SocketState
    {
        StartUp,
        NoSSLAcknowledged,
    };

    static std::string format(SocketState state)
    {
        switch (state)
        {
        case SocketState::StartUp:
            return "StartUp";
        case SocketState::NoSSLAcknowledged:
            return "NoSSLAcknowledged";
        default:
            return "Unknown";
        }
    }

    // std::ostream &operator<<(std::ostream &os, SocketState state)
    // {
    //     return os << static_cast<int>(state);
    // }

private:
    std::unordered_map<int, SocketState> socket_states;

    // Static pointer to the Singleton instance
    static SocketsManager *instancePtr;

    // Mutex to ensure thread safety
    static std::mutex mtx;

    // Private Constructor
    SocketsManager() {}

public:
    // Deleting the copy constructor to prevent copies
    SocketsManager(const SocketsManager &obj) = delete;

    // Static method to get the Singleton instance
    static SocketsManager *getInstance()
    {
        if (instancePtr == nullptr)
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (instancePtr == nullptr)
            {
                instancePtr = new SocketsManager();
            }
        }
        return instancePtr;
    }

    // TODO: protect the access to the socket_states map with a mutex
    static SocketState get_socket_state(int sockfd)
    {
        auto instance = getInstance();

        auto it = instance->socket_states.find(sockfd);
        if (it == instance->socket_states.end())
        {
            // set the initial state
            instance->socket_states[sockfd] = SocketState::StartUp;
            return SocketState::StartUp;
        }
    }

    // TODO: protect the access to the socket_states map with a mutex
    static void set_socket_state(int sockfd, SocketState state)
    {
        auto instance = getInstance();
        instance->socket_states[sockfd] = state;
    }
};

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug);

    // ref:
    // - https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#pattern-flags
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%@] %v");

    if (argc < 2)
    {
        SPDLOG_INFO("Please give a port number: ./epoll_echo_server [port]\n");
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
    SPDLOG_INFO("epoll echo server listening for connections on port: {}", portno);

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
            SPDLOG_DEBUG("new event, fd: {}, sock_listen_fd: {}", event_fd, sock_listen_fd);

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

                auto state = SocketsManager::get_socket_state(newsockfd);
                switch (state)
                {
                case SocketsManager::SocketState::StartUp:
                    SSLRequest::handle_ssl_request(newsockfd);
                    break;

                default:
                    SPDLOG_DEBUG("unknown socket state: {}", SocketsManager::format(state));
                    break;
                }

                int bytes_received = recv(newsockfd, buffer, MAX_MESSAGE_LEN, 0);

                if (bytes_received < 0)
                {
                    // spdlog::error("Error receiving data: {}", strerror(errno));
                    // close(newsockfd);
                    // continue;

                    // TODO:
                    // - add case "EAGAIN", which is the same as "EWOULDBLOCK"
                    switch (errno)
                    {
                    case EWOULDBLOCK:
                        // Non-blocking socket operation would block
                        SPDLOG_DEBUG("Would block, try again later");
                        break;
                    case ECONNREFUSED:
                        SPDLOG_DEBUG("Connection refused");
                        // Handle reconnection logic here
                        break;
                    case ETIMEDOUT:
                        SPDLOG_DEBUG("Connection timed out");
                        // Handle timeout logic here
                        break;
                    case ENOTCONN:
                        SPDLOG_DEBUG("Socket is not connected");
                        // Handle disconnection logic here
                        break;
                    default:
                        SPDLOG_DEBUG("recv() failed: {}", strerror(errno));
                        // Handle other errors
                        break;
                    }
                    continue;
                }

                // log the message in hex format
                SPDLOG_DEBUG("received[{} bytes]: {}", bytes_received, spdlog::to_hex(buffer, buffer + bytes_received));

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
