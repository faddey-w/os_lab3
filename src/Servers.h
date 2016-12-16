#ifndef OS_LAB3_SERVERS_HPP
#define OS_LAB3_SERVERS_HPP

#include <string>
#include <stdexcept>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>


class Server {

    static const struct sockaddr_in empty_addr;

    int listed_sock_fd;
    int listen_port;
    sockaddr_in serv_addr;
    bool init_done = false;

public:
    Server(int port)
        : listen_port(port), serv_addr(empty_addr), listed_sock_fd(0) {};

    void init();

    virtual std::string process(const std::string &request) = 0;

    void listen_once();

    void listen_forever();

    virtual ~Server();

};


class AuthServer : public Server {

    std::map<std::string, std::string> credentials;

    bool parse_cred_pair(std::string line,
                         std::string& username, std::string& password);


public:
    AuthServer(const std::string& auth_file_path, int port);

    std::string process(const std::string& request);
};



#endif //OS_LAB3_SERVERS_HPP
