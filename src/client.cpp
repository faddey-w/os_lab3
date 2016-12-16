#include <iostream>
#include "Clients.h"


int parse_int_or_fail(const std::string& str) {
    try{
        return std::stoi(str);
    } catch(const std::invalid_argument&) {
        throw std::runtime_error("Can not parse integer value");
    }
}

int main(int argc, char **argv) {
    
    try{
        if (argc < 3) {
            throw std::runtime_error("Usage: ./client <hostname> <port>");
        }
        std::string hostname(argv[1]);
        int port = parse_int_or_fail(argv[2]);
        Client client{hostname, port};
        
        std::string input;
        while(true) {
            std::getline(std::cin, input);
            std::cout << client.do_request(input) << std::endl;
        }
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
    return 0;
}