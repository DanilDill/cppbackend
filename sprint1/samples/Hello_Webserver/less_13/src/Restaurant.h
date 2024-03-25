#ifdef WIN32
#include <sdkddkver.h>
#endif


#include "Hamburger.h"
#include "Order.h"


// Функция, которая будет вызвана по окончании обработки заказа
//using OrderHandler = std::function<void(sys::error_code ec, int id, Hamburger* hamburger)>;

class Restaurant {
public:
    explicit Restaurant(net::io_context& io)
            : io_(io) {
    }

    int MakeHamburger(bool with_onion, OrderHandler handler)
    {
        const int order_id = ++next_order_id_;
        std::make_shared<Order>(io_, order_id, with_onion, std::move(handler))->Execute();
        return order_id;
    }

private:
    net::io_context& io_;
    int next_order_id_ = 0;
};

