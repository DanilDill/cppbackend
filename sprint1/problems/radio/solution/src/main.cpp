#include "audio.h"
#include "server.h"
#include "client.h"
#include <iostream>

using namespace std::literals;

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Usage: "sv << argv[0] << " <server or client >"sv << std::endl;
        return 1;
    }
    std::string instance = argv[1];
    if (instance != "server" and instance != "client")
    {
        std::cout << "Usage: "sv << argv[0] << " <server or client >"sv << std::endl;
        return 1;
    }

    if (instance == "server")
    {
        UDP::Server server(3333);
        server.start();
    }

    if (instance == "client")
    {
        UDP::Client client(3334);
        client.start("127.0.0.1",3333);
    }



    return 0;
}
