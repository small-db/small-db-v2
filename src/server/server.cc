// Copyright 2025 Xiaochen Cui
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// =====================================================================
// c std
// =====================================================================

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// =====================================================================
// c++ std
// =====================================================================

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// =====================================================================
// third-party libraries
// =====================================================================

// absl
#include "absl/base/options.h"

// pg_query
#include "pg_query.h"
#include "pg_query.pb-c.h"

// spdlog
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/spdlog.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/insert/insert.h"
#include "src/schema/schema.h"
#include "src/semantics/check.h"
#include "src/server/stmt_handler.h"
#include "src/server_base/args.h"
#include "src/server_registry/server_registry.h"
#include "src/util/ip/ip.h"

// =====================================================================
// self header
// =====================================================================

#include "src/server/server.h"

#define BACKLOG 512
#define MAX_EVENTS 128
#define MAX_MESSAGE_LEN 2048

namespace server {

std::atomic<bool> stopSignal = false;

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

    // Static pointer to the Singleton instance.
    static SocketsManager* instancePtr;

    // Mutex to ensure thread safety.
    static std::mutex mtx;

    // Private Constructor
    SocketsManager() {}

   public:
    /**
     * Delete the assignment operator.
     */
    void operator=(const SocketsManager&) = delete;

    /**
     * Delete the copy constructor.
     */
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

    static void remove_socket_state(int sockfd) {
        auto instance = getInstance();
        instance->socket_states.erase(sockfd);
    }
};

// define the static members
SocketsManager* SocketsManager::instancePtr = nullptr;
std::mutex SocketsManager::mtx;

// get a message with length word from connection
std::string pq_getmessage(char* buffer) {
    int len = read_int32_chars(buffer);
    std::string message(buffer + 4, len);
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
    void append_char(std::vector<char>& buffer, char value) {
        buffer.push_back(value);
    }

    void append_int16(std::vector<char>& buffer, int16_t value) {
        int16_t network_value = htons(value);
        const char* data = reinterpret_cast<const char*>(&network_value);
        buffer.insert(buffer.end(), data, data + sizeof(network_value));
    }

    void append_int32(std::vector<char>& buffer, int32_t value) {
        int32_t network_value = htonl(value);
        const char* data = reinterpret_cast<const char*>(&network_value);
        buffer.insert(buffer.end(), data, data + sizeof(network_value));
    }

    void write_int32(std::vector<char>& buffer, int32_t value, int offset) {
        int32_t network_value = htonl(value);
        const char* data = reinterpret_cast<const char*>(&network_value);
        memcpy(buffer.data() + offset, data, sizeof(network_value));
    }

    void append_cstring(std::vector<char>& buffer, const std::string& value) {
        buffer.insert(buffer.end(), value.begin(), value.end());
        buffer.push_back('\x00');
    }

    void append_vector(std::vector<char>& buffer,
                       const std::vector<char>& value) {
        buffer.insert(buffer.end(), value.begin(), value.end());
    }

   public:
    virtual void encode(std::vector<char>& buffer) = 0;
};

class AuthenticationOk : public Message {
   public:
    AuthenticationOk() = default;
    void encode(std::vector<char>& buffer) {
        append_char(buffer, 'R');
        append_int32(buffer, 8);
        append_int32(buffer, 0);
    }
};

class EmptyQueryResponse : public Message {
   public:
    EmptyQueryResponse() = default;
    void encode(std::vector<char>& buffer) {
        append_char(buffer, 'I');
        append_int32(buffer, 4);
    }
};

class RowDescriptionResponse : public Message {
   private:
    const std::shared_ptr<arrow::Schema>& schema;

   public:
    explicit RowDescriptionResponse(
        const std::shared_ptr<arrow::Schema>& schema)
        : schema(schema) {}

    void encode(std::vector<char>& buffer) {
        append_char(buffer, 'T');

        // message length (placeholder)
        int pre_bytes = buffer.size();
        append_int32(buffer, 0);

        int16_t num_fields = schema->num_fields();
        append_int16(buffer, num_fields);

        for (int i = 0; i < num_fields; ++i) {
            const auto& field = schema->field(i);

            auto data_type =
                small::type::from_string(field->type()->ToString().c_str())
                    .value();

            // The field name.
            append_cstring(buffer, field->name());

            // The table OID.
            append_int32(buffer, 0);

            // The column attribute number.
            append_int16(buffer, 0);

            // The field's data type OID.
            append_int32(buffer, small::type::to_pgwire_oid(data_type));

            // The data type size.
            append_int16(buffer, small::type::get_pgwire_size(data_type));

            // The type modifier.
            append_int32(buffer, 0);

            // The format code. (0 for text, 1 for binary)
            append_int16(buffer, 0);
        }

        // update the message length
        int32_t message_length = buffer.size() - pre_bytes;
        write_int32(buffer, message_length, pre_bytes);
    }
};

// DataRow (B)
class DataRowResponse : public Message {
   private:
    const std::shared_ptr<arrow::RecordBatch>& batch;

   public:
    explicit DataRowResponse(const std::shared_ptr<arrow::RecordBatch>& batch)
        : batch(batch) {}

    void encode(std::vector<char>& buffer) {
        int num_rows = batch->num_rows();

        for (int i = 0; i < num_rows; ++i) {
            append_char(buffer, 'D');

            // message length (placeholder)
            int pre_bytes = buffer.size();
            append_int32(buffer, 0);

            // number of columns
            append_int16(buffer, batch->num_columns());

            for (int j = 0; j < batch->num_columns(); ++j) {
                auto string_column =
                    std::static_pointer_cast<arrow::StringArray>(
                        batch->column(j));
                auto cell = string_column->GetString(i);
                append_int32(buffer, cell.size());
                buffer.insert(buffer.end(), cell.data(),
                              cell.data() + cell.size());
            }

            // update the message length
            int32_t message_length = buffer.size() - pre_bytes;
            write_int32(buffer, message_length, pre_bytes);
        }
    }
};

class CommandCompleteResponse : public Message {
   public:
    CommandCompleteResponse() = default;

    void encode(std::vector<char>& buffer) {
        // DataRow (B)

        // identifier
        append_char(buffer, 'C');

        // message length (placeholder)
        int pre_bytes = buffer.size();
        append_int32(buffer, 0);

        // command tag
        append_cstring(buffer, "SELECT 0");

        // update the message length
        int32_t message_length = buffer.size() - pre_bytes;
        write_int32(buffer, message_length, pre_bytes);
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
    const std::string error_message;

   public:
    explicit ErrorResponse(const std::string& error_message = "error message")
        : severity(Severity::ERROR), error_message(error_message) {}

    explicit ErrorResponse(Severity severity = Severity::ERROR,
                           const std::string& error_message = "error message")
        : severity(severity), error_message(error_message) {}

    void encode(std::vector<char>& buffer) {
        append_char(buffer, 'E');

        std::vector<char> field_severity = encode_severity();
        std::vector<char> field_message = encode_message();

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
                     std::string(buffer.begin(), buffer.end()));
    }

    std::vector<char> encode_severity() {
        std::vector<char> buffer;

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

    std::vector<char> encode_message() {
        std::vector<char> buffer;

        append_char(buffer, 'M');
        append_cstring(buffer, error_message);

        return buffer;
    }
};

class ParameterStatus : public Message {
    const std::string key;
    const std::string value;

   public:
    ParameterStatus(const std::string& key, const std::string& value)
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
    void encode(std::vector<char>& buffer) {
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
    void encode(std::vector<char>& buffer) {
        append_char(buffer, 'K');
        append_int32(buffer, 12);

        int32_t process_id = getpid();
        append_int32(buffer, process_id);

        srand(time(nullptr));

        // TODO: use a thread-local seed
        unsigned int seed = 42;
        int32_t secret_key = rand_r(&seed);
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
    void encode(std::vector<char>& buffer) {
        append_char(buffer, 'Z');
        append_int32(buffer, 5);
        append_char(buffer, 'I');
    }
};

class NetworkPackage {
   private:
    std::vector<Message*> messages;

   public:
    NetworkPackage() = default;

    void add_message(Message* message) { messages.push_back(message); }

    void send_all(int sockfd) {
        std::vector<char> buffer;
        for (auto message : messages) {
            message->encode(buffer);
        }

        send(sockfd, buffer.data(), buffer.size(), 0);
    }
};

void sendEmptyResult(int sockfd) {
    NetworkPackage* network_package = new NetworkPackage();
    network_package->add_message(new EmptyQueryResponse());
    network_package->add_message(new ReadyForQuery());
    network_package->send_all(sockfd);
}

void sendBatch(int sockfd, const std::shared_ptr<arrow::RecordBatch>& batch) {
    NetworkPackage* network_package = new NetworkPackage();
    network_package->add_message(new RowDescriptionResponse(batch->schema()));
    network_package->add_message(new DataRowResponse(batch));
    network_package->add_message(new CommandCompleteResponse());
    network_package->add_message(new ReadyForQuery());
    network_package->send_all(sockfd);
}

void sendError(int sockfd, const std::string& error_message) {
    NetworkPackage* network_package = new NetworkPackage();
    network_package->add_message(new ErrorResponse(error_message));
    network_package->add_message(new ReadyForQuery());
    network_package->send_all(sockfd);
}

void handle_query(std::string& query, int sockfd) {
    SPDLOG_INFO("query: {}", query);

    PgQueryParseResult result;

    result = pg_query_parse(query.c_str());
    if (result.error != nullptr) {
        SPDLOG_ERROR("error parsing query: {}", result.error->message);
        sendError(sockfd, result.error->message);
        return;
    }
    SPDLOG_INFO("ast: {}", result.parse_tree);
    pg_query_free_parse_result(result);

    PgQueryProtobufParseResult pgquery_pbparse_result =
        pg_query_parse_protobuf_opts(query.c_str(), PG_QUERY_PARSE_DEFAULT);

    auto unpacked = pg_query__parse_result__unpack(
        NULL, pgquery_pbparse_result.parse_tree.len,
        (const uint8_t*)pgquery_pbparse_result.parse_tree.data);

    auto node_case = unpacked->stmts[0]->stmt->node_case;

    for (int i = 0; i < unpacked->n_stmts; i++) {
        auto result = stmt_handler::handle_stmt(unpacked->stmts[i]->stmt);
        if (!result.ok()) {
            SPDLOG_ERROR("error handling statement: {}",
                         result.status().ToString());
            sendError(sockfd, result.status().ToString());
            return;
        }

        auto record_batch = result.value();

        if (record_batch->num_rows() == 0) {
            sendEmptyResult(sockfd);
            return;
        } else {
            SPDLOG_INFO("result batch: {}", record_batch->ToString());
            sendBatch(sockfd, record_batch);
            return;
        }
    }
}

void start_grpc_server(
    const std::string& addr,
    const std::vector<std::shared_ptr<grpc::Service>>& services) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

    for (const auto& service : services) {
        builder.RegisterService(service.get());
    }

    auto server = builder.BuildAndStart();
    std::thread([server = std::move(server), addr]() mutable {
        SPDLOG_INFO("server started, address: {}", addr);
        server->Wait();
        SPDLOG_INFO("server stopped, address: {}", addr);
    }).detach();
}

int RunServer(const small::server_base::ServerArgs& args) {
    auto status = small::server_base::init(args);
    if (!status.ok()) {
        SPDLOG_ERROR("failed to init server: {}", status.ToString());
        return EXIT_FAILURE;
    }

    SPDLOG_INFO(
        "start server: sql_address: {}, rpc_address: {}, region: {}"
        " data_dir: {}",
        args.sql_addr, args.grpc_addr, args.region, args.data_dir);

    start_grpc_server(
        args.grpc_addr,
        {
            std::make_shared<small::server_registry::RegistryService>(),
            std::make_shared<insert::InsertService>(),
        });

    status = small::server_registry::join(args);
    if (!status.ok()) {
        SPDLOG_ERROR("failed to join peer: {}", status.ToString());
        return EXIT_FAILURE;
    }

    struct sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    char buffer[MAX_MESSAGE_LEN];
    memset(buffer, 0, sizeof(buffer));

    int sock_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_listen_fd < 0) {
        SPDLOG_ERROR("error creating socket: {}", strerror(errno));
        exit(EXIT_FAILURE);  // Exit the program if socket creation fails
    }
    int opt = 1;
    setsockopt(sock_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    auto server_addr = small::util::ip::str_to_sockaddr(args.sql_addr);

    // bind socket and listen for connections
    if (bind(sock_listen_fd, (struct sockaddr*)&server_addr,
             sizeof(server_addr)) < 0) {
        std::string error_msg =
            fmt::format("error binding socket: {}", strerror(errno));
        SPDLOG_ERROR(error_msg);
        return EXIT_FAILURE;
    }

    if (listen(sock_listen_fd, BACKLOG) < 0) {
        SPDLOG_ERROR("error listening: {}", strerror(errno));
    }
    SPDLOG_INFO("server listening on addr: {}", args.sql_addr);

    struct epoll_event ev, events[MAX_EVENTS];
    int new_events, sock_conn_fd, epollfd;

    epollfd = epoll_create(MAX_EVENTS);
    if (epollfd < 0) {
        SPDLOG_ERROR("SPDLOG_ERROR creating epoll..\n");
    }
    ev.events = EPOLLIN;
    ev.data.fd = sock_listen_fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_listen_fd, &ev) == -1) {
        SPDLOG_ERROR("SPDLOG_ERROR adding new listeding socket to epoll..\n");
    }

    while (1) {
        if (stopSignal.load()) {
            SPDLOG_INFO("stop signal received, stopping the server");
            break;
        }

        // timeout: 1000ms
        new_events = epoll_wait(epollfd, events, MAX_EVENTS, 1000);

        if (new_events == -1) {
            SPDLOG_ERROR("SPDLOG_ERROR in epoll_wait..\n");
        }

        for (int i = 0; i < new_events; ++i) {
            int event_fd = events[i].data.fd;

            if (events[i].data.fd == sock_listen_fd) {
                sock_conn_fd =
                    accept4(sock_listen_fd, (struct sockaddr*)&client_addr,
                            &client_len, SOCK_NONBLOCK);
                if (sock_conn_fd == -1) {
                    SPDLOG_ERROR("SPDLOG_ERROR accepting new connection..\n");
                }

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = sock_conn_fd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_conn_fd, &ev) ==
                    -1) {
                    SPDLOG_ERROR("SPDLOG_ERROR adding new event to epoll..\n");
                }
            } else {
                int newsockfd = events[i].data.fd;

                auto state = SocketsManager::get_socket_state(newsockfd);
                switch (state) {
                    case SocketsManager::SocketState::StartUp: {
                        SSLRequest::handle_ssl_request(newsockfd);
                        SocketsManager::set_socket_state(
                            newsockfd,
                            SocketsManager::SocketState::NoSSLAcknowledged);
                        break;
                    }

                    case SocketsManager::SocketState::NoSSLAcknowledged: {
                        int bytes_received =
                            recv(newsockfd, buffer, MAX_MESSAGE_LEN, 0);

                        if (bytes_received < 0) {
                            // TODO(xiaochen):
                            // - add case "EAGAIN", which is the same as
                            // "EWOULDBLOCK"
                            switch (errno) {
                                case EWOULDBLOCK:
                                    // Non-blocking socket operation would block
                                    SPDLOG_DEBUG(
                                        "Would block, try again later");
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
                                    SPDLOG_DEBUG("recv() failed: {}",
                                                 strerror(errno));
                                    // Handle other errors
                                    break;
                            }
                            continue;
                        }

                        std::string message = pq_getmessage(buffer);

                        // the first 4 bytes is version
                        std::string version(buffer + 4, 4);

                        std::unordered_map<std::string, std::string>
                            recv_params;
                        // key and value are separated by '\x00'
                        int pos = 8;  // start after the version
                        while (pos < bytes_received) {
                            std::string key;
                            std::string value;

                            // Read key
                            while (pos < bytes_received &&
                                   buffer[pos] != '\x00') {
                                key += buffer[pos];
                                pos++;
                            }
                            pos++;  // skip the null character

                            // Read value
                            while (pos < bytes_received &&
                                   buffer[pos] != '\x00') {
                                value += buffer[pos];
                                pos++;
                            }
                            pos++;  // skip the null character

                            if (!key.empty()) {
                                recv_params[key] = value;
                            }
                        }

                        NetworkPackage* network_package = new NetworkPackage();
                        network_package->add_message(new AuthenticationOk());

                        std::unordered_map<std::string, std::string> params{
                            {"server_encoding", "UTF8"},
                            {"client_encoding", "UTF8"},
                            {"DateStyle", "ISO YMD"},
                            {"integer_datetimes", "on"},
                            {"server_version", "17.0"},
                        };
                        for (const auto& kv : params) {
                            network_package->add_message(
                                new ParameterStatus(kv.first, kv.second));
                        }
                        network_package->add_message(new BackendKeyData());
                        network_package->add_message(new ReadyForQuery());

                        network_package->send_all(newsockfd);
                        SocketsManager::set_socket_state(
                            newsockfd,
                            SocketsManager::SocketState::ReadyForQuery);
                        break;
                    }
                    case SocketsManager::SocketState::ReadyForQuery: {
                        std::vector<char> buffer(MAX_MESSAGE_LEN);
                        int bytes_received =
                            recv(newsockfd, buffer.data(), buffer.size(), 0);
                        if (bytes_received < 0) {
                            spdlog::error("error receiving data: {}",
                                          strerror(errno));
                            close(newsockfd);
                            continue;
                        } else if (bytes_received == 0) {
                            spdlog::info("connection closed by peer");
                            close(newsockfd);
                            continue;
                        }

                        char message_type = buffer[0];
                        switch (message_type) {
                            case 'Q': {
                                // Query

                                // read length of the query
                                int32_t query_len =
                                    read_int32_chars(buffer.data() + 1);

                                // read the query
                                std::string query(buffer.data() + 5,
                                                  query_len - 4);
                                handle_query(query, newsockfd);
                                break;
                            }

                            case 'X': {
                                // Terminate
                                SPDLOG_INFO("terminate connection");
                                close(newsockfd);
                                SocketsManager::remove_socket_state(newsockfd);
                                break;
                            }

                            default:
                                SPDLOG_ERROR("unknown message type: {}",
                                             message_type);
                                exit(EXIT_FAILURE);
                                break;
                        }
                        break;
                    }

                    default:
                        SPDLOG_ERROR("unknown socket state: {}",
                                     SocketsManager::format(state));
                        exit(EXIT_FAILURE);
                        break;
                }
            }
        }
    }

    close(sock_listen_fd);
    return 0;
}

void StopServer() {
    SPDLOG_INFO("stopping the server");
    stopSignal = true;
}

}  // namespace server
