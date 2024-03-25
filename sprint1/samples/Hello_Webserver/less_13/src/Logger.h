#include <chrono>
#include <syncstream>
#include <iostream>

using namespace std::chrono;
using namespace std::literals;

class Logger {
public:
    explicit Logger(std::string id)
            : id_(std::move(id)) {
    }

    void LogMessage(std::string_view message) const {
        std::osyncstream os{std::cout}; //c++20
        os << id_ << "> ["sv << duration<double>(steady_clock::now() - start_time_).count()
           << "s] "sv << message << std::endl;
    }

private:
    std::string id_;
    steady_clock::time_point start_time_{steady_clock::now()};
}; 