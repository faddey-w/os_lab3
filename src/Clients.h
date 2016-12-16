#ifndef OS_LAB3_CLIENTS_H
#define OS_LAB3_CLIENTS_H


#include <netinet/in.h>
#include <netdb.h>
#include <string>

class Client {

    hostent *server;
    sockaddr_in serv_addr;
    int port;

public:

    Client(const std::string& hostname, int port);

    std::string do_request(const std::string&);

};


#endif //OS_LAB3_CLIENTS_H
