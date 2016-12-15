#include <iostream>

#include "Servers.h"


int parse_int_or_fail(const std::string& str) {
    try{
        return std::stoi(str);
    } catch(const std::invalid_argument&) {
        throw std::runtime_error("Can not parse integer value");
    }
}




int main(int argc, char *argv[]) {

    try {
        if (argc < 3) {
            throw std::runtime_error("Usage: ./server <auth_sile> <port>");
        }
        AuthServer server(argv[1], parse_int_or_fail(argv[2]));
        server.init();
        server.listen_forever();

    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
    return 0;
}