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

Server::~Server() {
    if (listed_sock_fd) close(listed_sock_fd);
}


std::string trim(const std::string& line) {
    unsigned long first_no_space = line.find_first_not_of(" \n\t");
    unsigned long last_no_space = line.find_last_not_of(" \n\t");
    return line.substr(first_no_space, last_no_space-first_no_space+1);
}


bool AuthServer::parse_cred_pair(std::string line,
                                 std::string &username, std::string &password) {
    line = trim(line);
    unsigned long first_space = line.find_first_of(" \n\t");
    if (first_space == -1) {
        return false;
    }
    username = line.substr(0, first_space);
    line = line.substr(first_space);
    password = trim(line);
    if (password.find_first_of(" \n\t") != -1) {
        return false;
    }
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


