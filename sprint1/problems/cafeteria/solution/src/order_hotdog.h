#pragma once
#include <boost/asio/bind_executor.hpp>
#include "hotdog.h"
#include "result.h"
#include "logger.h"
#include "thread_checker.h"
// Функция-обработчик операции приготовления хот-дога
using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;
using BreadHandler = std::function<void(std::shared_ptr<Bread>&&)>;
using BreadBackingHandler = std::function<void()>;


using Timer = net::steady_timer;
using namespace std::literals;



class BreadOrder : public std::enable_shared_from_this<BreadOrder>
{
public:
    BreadOrder (net::strand<net::io_context::executor_type>& store_strand, Logger& logger, Store& store, BreadHandler handler):
    _handler(std::move(handler)),
    _store_strand(store_strand),
    _store(store),
    _logger(logger)
    {};

   void Execute()
    {
       net::defer(_store_strand,[self = shared_from_this() ]()
       {
           auto bread = self->_store.GetBread();
           self->_logger.LogMessage("store get Bread #"s + std::to_string(bread->GetId()));
           self->_handler(std::move(bread));
       });
    }
private:
    BreadHandler _handler;
    Store& _store;
    Logger& _logger;
    net::strand<net::io_context::executor_type>& _store_strand;
};

class BreadBakingOrder : public std::enable_shared_from_this<BreadBakingOrder>
{
public:
    BreadBakingOrder(net::io_context& context,Logger& logger ,std::shared_ptr<Bread>& bread, GasCooker& cooker,BreadBackingHandler handler):
    _context(context),
    _bread(bread),
    _cooker(cooker),
    _handler(std::move(handler)),
    _logger(logger)
    {};
    void Execute()
    {
        net::post(strand_baking,[self = shared_from_this() ]()
        {
            self->_logger.LogMessage("Bread #"s + std::to_string(self->_bread->GetId()) + "has start backed ");
            self->_bread->StartBake(self->_cooker,[](){});
        });
        _timer_.async_wait(net::bind_executor(strand_baking,[self = shared_from_this()](sys::error_code ec)
        {
            self->OnBaking();
        }));

    }
    void OnBaking()
    {
        _bread ->StopBaking();
        _logger.LogMessage("Bread #"s + std::to_string(_bread->GetId()) + " has been backed ");
        _handler();
    }
private:
    net::io_context& _context;
    std::shared_ptr<Bread>& _bread;
    GasCooker& _cooker;
    Logger& _logger;
    net::strand<net::io_context::executor_type> strand_baking{net::make_strand(_context)};
    BreadBackingHandler _handler;
    Timer _timer_{_context,1s};
};
class Order : public std::enable_shared_from_this<Order>
{
public:
    Order(net::io_context& io, net::strand<net::io_context::executor_type>&strand_store, size_t id, Store& store, GasCooker& cooker,HotDogHandler handler):
    _context{io},
    _strand_store(strand_store),
    _store(store),
     id_(id),
    _cooker{cooker},
    _handler(std::move(handler))
    {}

    void Execute()
    {

       std::make_shared<BreadOrder>(_strand_store,logger_ ,_store, [self = shared_from_this()](std::shared_ptr<Bread>&& bread)
       {
           self->_bread = std::move(bread);
           std::make_shared<BreadBakingOrder>(self->_context, self->logger_ ,self->_bread, self->_cooker,[self]()
           {
              self->logger_.LogMessage("Bread #" + std::to_string(self->_bread->GetId())+ " is ready");
              self->bread_ready = true;
              self->CheckReadness();
           })->Execute();

       })->Execute();

    }

    void CheckReadness()
    {
        if (bread_ready && sausage_ready)
        {
            try {
                    HotDog hotdog(id_, _sausage, _bread);
                    Result<HotDog> result_hotdog(std::move(hotdog));
                    _handler(std::move(result_hotdog));
                }
            catch (...)
            {
                auto result = Result<HotDogHandler>::FromCurrentException();
            }
        }
    }
private:
    size_t id_;
    net::io_context& _context;
    net::strand<net::io_context::executor_type>& _strand_store;
    std::shared_ptr<Bread>_bread;
    std::shared_ptr<Sausage> _sausage;
    bool bread_ready = false;
    bool sausage_ready=false;
    HotDogHandler _handler;
    GasCooker& _cooker;
    Store& _store ;

    Logger logger_{std::to_string(id_)};
    Timer sausage_timer_{_context,1500ms};

};