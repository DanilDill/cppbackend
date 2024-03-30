#include "ingredients.h"

using SausageHandler = std::function<void(std::shared_ptr<Sausage>&&)>;
using SausageFryingHandler = std::function<void()>;
using Timer = net::steady_timer;


class SausageOrder : public std::enable_shared_from_this<SausageOrder>
{
public:
    SausageOrder (net::strand<net::io_context::executor_type>& store_strand, Logger& logger, Store& store, SausageHandler handler):
            _handler(std::move(handler)),
            _store_strand(store_strand),
            _store(store),
            _logger(logger)
    {};

    void Execute()
    {
        net::defer(_store_strand,[self = shared_from_this() ]()
        {
            auto sausage = self->_store.GetSausage();
            self->_logger.LogMessage("store get Susage #"s + std::to_string(sausage->GetId()));
            self->_handler(std::move(sausage));
        });
    }
private:
    SausageHandler _handler;
    Store& _store;
    Logger& _logger;
    net::strand<net::io_context::executor_type>& _store_strand;
};

class SausageFryingOrder : public std::enable_shared_from_this<SausageFryingOrder>
{
public:
    SausageFryingOrder(net::io_context& context,Logger& logger ,std::shared_ptr<Sausage>& bread, GasCooker& cooker,BreadBackingHandler handler):
            _context(context),
            _sausage(bread),
            _cooker(cooker),
            _handler(std::move(handler)),
            _logger(logger)
    {};
    void Execute()
    {

        net::post(strand_baking,[self = shared_from_this() ]()
        {
            self->_logger.LogMessage("Sausage #"s + std::to_string(self->_sausage->GetId()) + "has start frying ");
            self->_sausage->StartFry(self->_cooker,[self]()
            {
                self->_timer_.async_wait(net::bind_executor(self->strand_baking,[self](sys::error_code ec)
                {
                    self->OnBaking();
                }));
            });

        });
    }
    void OnBaking()
    {
        net::post(strand_baking,[self = shared_from_this()]()
        {
            self->_sausage ->StopFry();
            self->_logger.LogMessage("Sausage #"s + std::to_string(self->_sausage->GetId()) + " has been frying ");
            self->_handler();
        });

    }
private:
    net::io_context& _context;
    std::shared_ptr<Sausage>& _sausage;
    GasCooker& _cooker;
    Logger& _logger;
    net::strand<net::io_context::executor_type> strand_baking{net::make_strand(_context)};
    SausageFryingHandler _handler;
    Timer _timer_{_context,1510ms};
};