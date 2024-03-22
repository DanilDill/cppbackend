#include "tcp_instance.h"

namespace TCP
{
    namespace SeaBattle
    {
        class Client : public TCPInstance
        {
        public:
            Client(std::string& host,size_t port):TCPInstance(host,port);
        };
    }
}