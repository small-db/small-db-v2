#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pg_query.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <mutex>
#include <unordered_map>

// set level for "SPDLOG_<LEVEL>" macros
// NB: must define SPDLOG_ACTIVE_LEVEL before `#include "spdlog/spdlog.h"`
// to make it works
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/fmt/bin_to_hex.h>  // spdlog::to_hex (doesn't work in C++20 and later version)
#include <spdlog/spdlog.h>

#include "src/query/query.h"
#include "src/server/server.h"
#include "src/store/store.h"

#define BACKLOG 512
#define MAX_EVENTS 128
#define MAX_MESSAGE_LEN 2048

using namespace std;

class ReaderWriter {
   protected:
    static int32_t read_int32(int sockfd) {
        int32_t network_value;
        ssize_t bytes_received =
            recv(sockfd, &network_value, sizeof(network_value), 0);
        if (bytes_received != sizeof(network_value)) {
            throw std::runtime_error("error reading int32_t from socket..");
        }
        int32_t value = ntohl(network_value);
        return value;
    }
};

int32_t read_int32_chars(char* buffer) {
    int32_t network_value;
    memcpy(&network_value, buffer, sizeof(network_value));
    int32_t value = ntohl(network_value);
    return value;
}

class SocketsManager {
   public:
    enum class SocketState {
        StartUp,
        NoSSLAcknowledged,
        ReadyForQuery,
    };

    static std::string format(SocketState state) {
        switch (state) {
            case SocketState::StartUp:
                return "StartUp";
            case SocketState::NoSSLAcknowledged:
                return "NoSSLAcknowledged";
            default:
                return "Unknown";
        }
    }

   private:
    std::unordered_map<int, SocketState> socket_states;

    // Static pointer to the Singleton instance
    static SocketsManager* instancePtr;

    // Mutex to ensure thread safety
    static std::mutex mtx;

    // Private Constructor
    SocketsManager() {}

   public:
    // Deleting the copy constructor to prevent copies
    SocketsManager(const SocketsManager& obj) = delete;

    // Static method to get the Singleton instance
    static SocketsManager* getInstance() {
        if (instancePtr == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);
            if (instancePtr == nullptr) {
                instancePtr = new SocketsManager();
            }
        }
        return instancePtr;
    }

    // TODO: protect the access to the socket_states map with a mutex
    static SocketState get_socket_state(int sockfd) {
        auto instance = getInstance();

        auto it = instance->socket_states.find(sockfd);
        if (it == instance->socket_states.end()) {
            // set the initial state
            instance->socket_states[sockfd] = SocketState::StartUp;
            return SocketState::StartUp;
        }

        return it->second;
    }

    // TODO: protect the access to the socket_states map with a mutex
    static void set_socket_state(int sockfd, SocketState state) {
        auto instance = getInstance();
        instance->socket_states[sockfd] = state;
    }
};

// Define the static members
SocketsManager* SocketsManager::instancePtr = nullptr;
std::mutex SocketsManager::mtx;

// get a message with length word from connection
string pq_getmessage(char* buffer) {
    int len = read_int32_chars(buffer);
    string message(buffer + 4, len);
    return message;
}

class SSLRequest : ReaderWriter {
   public:
    static const int BODY_SIZE = 8;
    static const int SSL_MAGIC_CODE = 80877103;

    static void handle_ssl_request(int newsockfd) {
        SPDLOG_DEBUG("handling ssl request, newsockfd: {}", newsockfd);

        auto body_size = read_int32(newsockfd);
        if (body_size != BODY_SIZE) {
            auto error_msg =
                fmt::format("invalid length of startup packet: {}", body_size);
            throw std::runtime_error(error_msg);
        }

        auto ssl_code = read_int32(newsockfd);
        if (ssl_code != SSL_MAGIC_CODE) {
            auto error_msg = fmt::format("invalid ssl code: {}", ssl_code);
            throw std::runtime_error(error_msg);
        }

        // reply 'N' for no SSL support
        char SSLok = 'N';
        send(newsockfd, &SSLok, 1, 0);
    }
};

class Message {
   protected:
    void append_char(vector<char>& buffer, char value) {
        buffer.push_back(value);
    }

    void append_int32(vector<char>& buffer, int32_t value) {
        int32_t network_value = htonl(value);
        const char* data = reinterpret_cast<const char*>(&network_value);
        buffer.insert(buffer.end(), data, data + sizeof(network_value));
    }

    void append_cstring(vector<char>& buffer, const string& value) {
        buffer.insert(buffer.end(), value.begin(), value.end());
        buffer.push_back('\x00');
    }

    void append_vector(vector<char>& buffer, const vector<char>& value) {
        buffer.insert(buffer.end(), value.begin(), value.end());
    }

   public:
    virtual void encode(vector<char>& buffer) = 0;
};

class AuthenticationOk : public Message {
   public:
    AuthenticationOk() = default;
    void encode(vector<char>& buffer) {
        append_char(buffer, 'R');
        append_int32(buffer, 8);
        append_int32(buffer, 0);
    }
};

class EmptyQueryResponse : public Message {
   public:
    EmptyQueryResponse() = default;
    void encode(vector<char>& buffer) {
        append_char(buffer, 'I');
        append_int32(buffer, 4);
    }
};

class ErrorResponse : public Message {
    enum class Severity {
        ERROR,
        FATAL,
        PANIC,
        WARNING,
        NOTICE,
        DEBUG,
        INFO,
        LOG,
    };

   private:
    const Severity severity;
    const string error_message;

   public:
    ErrorResponse(Severity severity = Severity::ERROR,
                  const string& error_message = "error message")
        : severity(severity), error_message(error_message) {}

    void encode(vector<char>& buffer) {
        append_char(buffer, 'E');

        vector<char> field_severity = encode_severity();
        vector<char> field_message = encode_message();

        int32_t message_length =
            4 + field_severity.size() + field_message.size() + 1;
        SPDLOG_DEBUG("message_length: {}", message_length);
        append_int32(buffer, message_length);
        append_vector(buffer, field_severity);
        append_vector(buffer, field_message);
        append_char(buffer, '\x00');

        SPDLOG_DEBUG(
            "error response: {}",
            spdlog::to_hex(buffer.data(), buffer.data() + buffer.size()));
        SPDLOG_DEBUG("error response: {}",
                     string(buffer.begin(), buffer.end()));
    }

    vector<char> encode_severity() {
        vector<char> buffer;

        append_char(buffer, 'S');
        switch (severity) {
            case Severity::DEBUG:
                append_cstring(buffer, "DEBUG");
                break;
            case Severity::INFO:
                append_cstring(buffer, "INFO");
                break;
            case Severity::ERROR:
                append_cstring(buffer, "ERROR");
                break;
            default:
                throw std::runtime_error("unsupported severity");
        }
        return buffer;
    }

    vector<char> encode_message() {
        vector<char> buffer;

        append_char(buffer, 'M');
        append_cstring(buffer, error_message);

        return buffer;
    }
};

class ParameterStatus : public Message {
    const string key;
    const string value;

   public:
    ParameterStatus(const string& key, const string& value)
        : key(key), value(value) {}

    // ParameterStatus (B)
    // Byte1('S')
    // Identifies the message as a run-time parameter status report.
    // Int32
    // Length of message contents in bytes, including self.
    // String
    // The name of the run-time parameter being reported.
    // String
    // The current value of the parameter.
    void encode(vector<char>& buffer) {
        append_char(buffer, 'S');
        append_int32(buffer, 4 + key.size() + 1 + value.size() + 1);
        append_cstring(buffer, key);
        append_cstring(buffer, value);
    }
};

class BackendKeyData : public Message {
   public:
    BackendKeyData() = default;

    // BackendKeyData (B)
    // Byte1('K')
    // Identifies the message as cancellation key data. The frontend must save
    // these values if it wishes to be able to issue CancelRequest messages
    // later. Int32(12) Length of message contents in bytes, including self.
    // Int32
    // The process ID of this backend.
    // Int32
    // The secret key of this backend.
    void encode(vector<char>& buffer) {
        append_char(buffer, 'K');
        append_int32(buffer, 12);

        int32_t process_id = getpid();
        append_int32(buffer, process_id);

        srand(time(nullptr));
        int32_t secret_key = rand();
        append_int32(buffer, secret_key);
    }
};

class ReadyForQuery : public Message {
   public:
    ReadyForQuery() = default;

    // ReadyForQuery (B)
    // Byte1('Z')
    // Identifies the message as a ready-for-query indicator.
    // Int32(5)
    // Length of message contents in bytes, including self.
    // Byte1
    // Current backend transaction status indicator. Possible values are 'I' if
    // idle (not in a transaction block); 'T' if in a transaction block; or 'E'
    // if in a failed transaction block (queries will be rejected until block is
    // ended).
    void encode(vector<char>& buffer) {
        append_char(buffer, 'Z');
        append_int32(buffer, 5);
        append_char(buffer, 'I');
    }
};

class NetworkPackage {
   private:
    vector<Message*> messages;

   public:
    NetworkPackage() = default;

    void add_message(Message* message) { messages.push_back(message); }

    void send_all(int sockfd) {
        vector<char> buffer;
        for (auto message : messages) {
            message->encode(buffer);
        }

        send(sockfd, buffer.data(), buffer.size(), 0);
    }
};

void sendUnimplemented(int sockfd) {
    NetworkPackage* network_package = new NetworkPackage();
    // network_package->add_message(new ErrorResponse());
    network_package->add_message(new EmptyQueryResponse());
    network_package->add_message(new ReadyForQuery());
    network_package->send_all(sockfd);
}

void handle_query(string& query, int sockfd) {
    SPDLOG_INFO("query: {}", query);

    PgQueryParseResult result;

    result = pg_query_parse(query.c_str());

    SPDLOG_INFO("parse_tree: {}", result.parse_tree);

    pg_query_free_parse_result(result);

    sendUnimplemented(sockfd);
}

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%@] %v");

    if (argc < 2) {
        SPDLOG_INFO("Please give a port number: ./epoll_echo_server [port]\n");
        exit(0);
    }

    int portno = strtol(argv[1], NULL, 10);
    server::ServerArgs args = {portno};
    return server::RunServer(args);
}
