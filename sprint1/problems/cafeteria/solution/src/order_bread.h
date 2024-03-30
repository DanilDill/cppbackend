#include <boost/asio/bind_executor.hpp>
#include "ingredients.h"
using BreadHandler = std::function<void(std::shared_ptr<Bread>&&)>;
using BreadBackingHandler = std::function<void()>;
using Timer = net::steady_timer;
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
            self->_bread->StartBake(self->_cooker,[self]()
            {
                net::dispatch(self->strand_baking,[self]()
                {
                    self->_logger.LogMessage("Bread #"s + std::to_string(self->_bread->GetId()) + "has start backed ");
                    self->_timer_.expires_after(1010ms);
                    self->_timer_.async_wait(net::bind_executor(self->strand_baking,[self](sys::error_code ec)
                    {
                        self->OnBaking();
                    }));
                });

            });
        });


    }
    void OnBaking()
    {
        net::post(strand_baking,[self = shared_from_this()]()
        {
            self->_bread ->StopBaking();
            self->_logger.LogMessage("Bread #"s + std::to_string(self->_bread->GetId()) + " has been backed ");
            self->_handler();
        });

    }
private:
    net::io_context& _context;
    std::shared_ptr<Bread>& _bread;
    GasCooker& _cooker;
    Logger& _logger;
    net::strand<net::io_context::executor_type> strand_baking{net::make_strand(_context)};
    BreadBackingHandler _handler;
    Timer _timer_{_context};
};