#pragma once
#include <boost/asio/bind_executor.hpp>
#include "hotdog.h"
#include "result.h"

// Функция-обработчик операции приготовления хот-дога
using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;
using Timer = net::steady_timer;
using namespace std::literals;



class Order : public std::enable_shared_from_this<Order>
{
public:
    Order(net::io_context& io, Store& store, GasCooker& cooker,HotDogHandler handler):
    _context{io},
    _store(store),
    _cooker{cooker},
    _handler(std::move(handler))
    {}

    void Execute()
    {
       auto bread =  CookBread();
       auto sausage = CookSausage();

    }

private:
    std::shared_ptr<Sausage>  CookSausage()
    {
        auto sausage = _store.GetSausage();
        net::post(strand_sausage,[&sausage, self = shared_from_this()]
        {
            sausage->StartFry(self->_cooker,[&self]()
            {
                self->onCookSausage();
            });
        });
        net::post(strand_sausage,[&sausage, self = shared_from_this()]()
        {
            sausage->StopFry();
        });
        return sausage;
    }
    void onCookSausage()
    {
        sausage_timer_.wait();
        sausage_ready = true;
    };
    std::shared_ptr<Bread>CookBread()
    {
         auto bread = _store.GetBread();
         net::post(strand_bread,[&bread, self = shared_from_this()]()
         {
            bread->StartBake(self->_cooker,[&self]()
            {
                self->onCookBread();
            });
         });
         net::post(strand_bread,[&bread,self = shared_from_this()]()
         {
             bread->StopBaking();

         });
        return bread;
    };

    void onCookBread()
    {
        bread_timer_.wait();
        bread_ready = true;
    };

private:
    bool bread_ready = false;
    bool sausage_ready=false;
    HotDogHandler _handler;
    GasCooker& _cooker;
    Store& _store ;
    net::io_context& _context;

    Timer bread_timer_{_context, 1000ms};
    Timer sausage_timer_{_context,1500ms};
    net::strand<net::io_context::executor_type> strand_bread{net::make_strand(_context)};
    net::strand<net::io_context::executor_type> strand_sausage{net::make_strand(_context)};
    //HotDog _hotDog;
};