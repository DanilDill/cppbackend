#pragma once
#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <memory>

#include "hotdog.h"
#include "result.h"
#include "order_hotdog.h"
namespace net = boost::asio;



// Класс "Кафетерий". Готовит хот-доги
class Cafeteria {
public:
    explicit Cafeteria(net::io_context& io)
        : io_{io} {
    }

    // Асинхронно готовит хот-дог и вызывает handler, как только хот-дог будет готов.
    // Этот метод может быть вызван из произвольного потока
    void OrderHotDog(HotDogHandler handler)
    {
        // TODO: Реализуйте метод самостоятельно
           const int order_id = ++_order_id;
          std::make_shared<Order>(io_,strand_store,order_id,store_,*gas_cooker_,std::move(handler))->Execute();
        //order_id = ++order_id;
        //std::make_shared<Order>(io_,order_id,store_,*gas_cooker_,std::move(handler))->Execute();
        // При необходимости реализуйте дополнительные классы
    }

private:
    std::atomic_int _order_id = 0;
    net::io_context& io_;

    net::strand<net::io_context::executor_type> strand_store{net::make_strand(io_)};
    // Используется для создания ингредиентов хот-дога
    Store store_;
    // Газовая плита. По условию задачи в кафетерии есть только одна газовая плита на 8 горелок
    // Используйте её для приготовления ингредиентов хот-дога.
    // Плита создаётся с помощью make_shared, так как GasCooker унаследован от
    // enable_shared_from_this.
    std::shared_ptr<GasCooker> gas_cooker_ = std::make_shared<GasCooker>(io_);
};
