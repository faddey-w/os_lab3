#include <unistd.h>
#include "Clients.h"

static const sockaddr_in empty_sockaddr{};


Client::Client(const std::string &hostname, int port)
: server(nullptr), port(port), serv_addr(empty_sockaddr) {
    server = gethostbyname(hostname.c_str());
    if (server == nullptr) {
        throw std::runtime_error("Error, no such host\n");
    }

    serv_addr.sin_family = AF_INET;
    memcpy(server->h_addr,
           (char *)&serv_addr.sin_addr.s_addr,
           server->h_length);
    serv_addr.sin_port = htons(port);
}

std::string Client::do_request(const std::string &message) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        throw std::runtime_error("Error opening socket");

    if (connect(socket_fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        throw std::runtime_error("Error on connecting");
    ssize_t n_bytes = write(socket_fd, message.c_str(), message.size());
    if (n_bytes < 0)
        throw std::runtime_error("Error writing to socket");
    char buffer[256] = {0};
    n_bytes = read(socket_fd,buffer,255);
    if (n_bytes < 0)
        throw std::runtime_error("Error reading from socket");
    if (socket_fd) close(socket_fd);

    return std::string(buffer, n_bytes);
}
