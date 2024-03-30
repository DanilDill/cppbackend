#pragma once
#include "hotdog.h"
#include "result.h"
#include "logger.h"
#include "thread_checker.h"
#include "order_bread.h"
#include "order_sausage.h"
// Функция-обработчик операции приготовления хот-дога
using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;



using Timer = net::steady_timer;
using namespace std::literals;



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

       std::make_shared<BreadOrder>(_strand_store, logger_ , _store, [self = shared_from_this()](std::shared_ptr<Bread>&& bread)
       {
           self->_bread = std::move(bread);
           std::make_shared<BreadBakingOrder>(self->_context, self->logger_ ,self->_bread, self->_cooker,[self]()
           {
              self->logger_.LogMessage("Bread #" + std::to_string(self->_bread->GetId())+ " is ready");
              self->bread_ready = true;
              self->CheckReadness();
           })->Execute();

       })->Execute();

        std::make_shared<SausageOrder>(_strand_store, logger_ , _store, [self = shared_from_this()](std::shared_ptr<Sausage>&& sausage)
        {
            self->_sausage = std::move(sausage);
            std::make_shared<SausageFryingOrder>(self->_context, self->logger_ ,self->_sausage, self->_cooker,[self]()
            {
                self->logger_.LogMessage("Sausage #" + std::to_string(self->_sausage->GetId())+ " is ready");
                self->sausage_ready = true;
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
                auto result = Result<HotDog>::FromCurrentException();
                _handler(std::move(result));
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

};