#include "Servers.h"


const sockaddr_in Server::empty_addr = sockaddr_in();


void Server::init() {
    // create a socket to listen
    listed_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listed_sock_fd < 0) {
        throw std::runtime_error("Can not open socket for listening");
    }

    // fill addr struct
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(listen_port);

    // bind socket to the address
    if (bind(listed_sock_fd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        throw std::runtime_error("Error on binding socket");
    }

    // mark this socket as listening with maximum 10 connections in the queue
    listen(listed_sock_fd, 10);

    init_done = true;
}


void Server::listen_once() {
    if (!init_done) {
        throw std::runtime_error("call init() before listening!");
    }

    sockaddr_in client_addr;
    socklen_t client_addr_struct_size = sizeof(client_addr);

    int client_sock_fd = accept(
        listed_sock_fd,
        (struct sockaddr *) &client_addr,
        &client_addr_struct_size
    );
    if (client_sock_fd < 0) {
        throw std::runtime_error("Error on accept");
    }
    char buffer[256] = {0};
    ssize_t n_bytes = read(client_sock_fd, buffer, 255);
    if (n_bytes < 0) {
        throw std::runtime_error("Error on reading from socket");
    }

    std::string request(buffer, n_bytes);
    std::string response = process(request);

    n_bytes = write(client_sock_fd, response.c_str(), response.size());
    if (n_bytes < 0) throw std::runtime_error("Error on writing to socket");

    close(client_sock_fd);
}

void Server::listen_forever() {
    while (true) {
        try {
            listen_once();
        } catch (const std::runtime_error &err) {
            std::cerr << err.what();
        }
    }
}


bool AuthServer::parse_cred_pair(const std::string &line,
                                 std::string username, std::string &password) {
    std::stringstream iss(line);
    std::getline(iss, username, ' ');
    if (!iss) {
        return false;
    }
    std::getline(iss, password, ' ');
    return true;
}


AuthServer::AuthServer(const std::string &auth_file_path, int port) : Server(port) {
    std::ifstream auth_file(auth_file_path);
    if (!auth_file.is_open()) {
        throw std::runtime_error("Can not open auth file");
    }

    std::string line;
    while (std::getline(auth_file, line)) {
        std::string username, password;
        if (!parse_cred_pair(line, username, password)) {
            throw std::runtime_error("Lines in auto file should have two words");
        }
        credentials[username] = password;
    }

    for (auto &item : credentials) {
        std::cout << item.first << " :: " << item.second << std::endl;
    }

}

std::string AuthServer::process(const std::string &request) {
    std::string username, password;
    if (!parse_cred_pair(request, username, password)) {
        return "BAD REQUEST";
    }
    if (credentials.count(username) > 0 && credentials[username] == password) {
        return "OK";
    } else {
        return "FAIL";
    }
}


