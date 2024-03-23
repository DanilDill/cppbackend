#ifdef WIN32
#include <sdkddkver.h>
#endif

#include "seabattle.h"

#include <atomic>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

void PrintFieldPair(const SeabattleField& left, const SeabattleField& right) {
    auto left_pad = "  "s;
    auto delimeter = "    "s;
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
    for (size_t i = 0; i < SeabattleField::field_size; ++i) {
        std::cout << left_pad;
        left.PrintLine(std::cout, i);
        std::cout << delimeter;
        right.PrintLine(std::cout, i);
        std::cout << std::endl;
    }
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
}

template <size_t sz>
static std::optional<std::string> ReadExact(tcp::socket& socket) {
    boost::array<char, sz> buf;
    boost::system::error_code ec;

    net::read(socket, net::buffer(buf), net::transfer_exactly(sz), ec);

    if (ec) {
        return std::nullopt;
    }

    return {{buf.data(), sz}};
}

static bool WriteExact(tcp::socket& socket, std::string_view data) {
    boost::system::error_code ec;

    net::write(socket, net::buffer(data), net::transfer_exactly(data.size()), ec);

    return !ec;
}

class SeabattleAgent
        {
public:
    SeabattleAgent(const SeabattleField& field)
        : my_field_(field) {
    }

    void StartGame(tcp::socket& socket, bool my_initiative)
    {

        boost::system::error_code ec;
        PrintFields();
        while (!IsGameEnded())
        {
            if (my_initiative)
            {

                std::string my_turn;
                std::cout << "Your turn : ";
                std::cin >> my_turn;
                auto your_parsed_data = ParseMove(my_turn);
                // B1 =
                std::cout << "Parsed turn : " << your_parsed_data ->first << " " << your_parsed_data->second<< std::endl;
                if(your_parsed_data)
                {
                    WriteExact(socket,MoveToString(*your_parsed_data));
                    auto shootResult = ReadExact<1>(socket);
                    switch (static_cast<SeabattleField::ShotResult>(stoi(*shootResult)))
                    {
                        case SeabattleField::ShotResult::MISS:
                            other_field_.MarkMiss(your_parsed_data->second, your_parsed_data->first);
                            my_initiative = !my_initiative;
                            std::cout<< "Miss\n";
                            break;
                        case SeabattleField::ShotResult::HIT:
                            other_field_.MarkHit(your_parsed_data->second, your_parsed_data->first);
                            std::cout << "Hit\n";
                            break;
                        case SeabattleField::ShotResult::KILL:
                            other_field_.MarkKill(your_parsed_data->second, your_parsed_data->first);
                            std::cout<<"Kill\n";
                            break;

                    }
                    PrintFields();

                }
                else
                {
                     std::cout << "Wrong input. Try again.\n";
                     continue;
                }
            }
            else
            {
                auto enemy_turn = ReadExact<2>(socket);
                auto parsed_enemy_data = ParseMove(*enemy_turn);
                if (parsed_enemy_data)
                {
                    std::cout << "Shot to " << *enemy_turn << std::endl;
                    auto result = my_field_.Shoot(parsed_enemy_data->second,parsed_enemy_data->first );
                    switch (result)
                    {
                        case SeabattleField::ShotResult::MISS:
                            my_field_.MarkMiss(parsed_enemy_data->second,parsed_enemy_data->first );
                            my_initiative= ! my_initiative;
                            WriteExact(socket,"0");
                            break;
                        case SeabattleField::ShotResult::HIT:
                            my_field_.MarkHit(parsed_enemy_data->second,parsed_enemy_data->first );
                            WriteExact(socket,"1");
                            break;
                        case SeabattleField::ShotResult::KILL:
                            my_field_.MarkKill(parsed_enemy_data->second,parsed_enemy_data->first );
                            WriteExact(socket,"2");
                            break;

                    }
                    PrintFields();

                }

            }

        }
        std::cout << "GAME OVER";

    }

private:
    static std::optional<std::pair<int, int>> ParseMove(const std::string_view& sv) {
        if (sv.size() != 2) return std::nullopt;

        int p1 = sv[0] - 'A', p2 = sv[1] - '1';

        if (p1 < 0 || p1 > 8) return std::nullopt;
        if (p2 < 0 || p2 > 8) return std::nullopt;

        return {{p1, p2}};
    }

    static std::string MoveToString(std::pair<int, int> move)
    {
        char first = static_cast<char>(move.first) + 'A';
        char second = static_cast<char>(move.second) + '1';
        char buff[] = {first, second};
        return {buff, 2};
    }

    void PrintFields() const {
        PrintFieldPair(my_field_, other_field_);
    }

    bool IsGameEnded() const {
        return my_field_.IsLoser() || other_field_.IsLoser();
    }


private:
    SeabattleField my_field_;
    SeabattleField other_field_;
};

void StartServer(const SeabattleField& field, unsigned short port)
{
    SeabattleAgent agent(field);
    net::io_context context;
    boost::asio::ip::tcp::endpoint endpoint(net::ip::make_address("127.0.0.1"), port);
    tcp::acceptor acceptor(context);
    acceptor.open(endpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen(1);
    std::cout << "Waiting for connection on "sv << "127.0.0.1" << ":" << port  << std::endl;
    boost::system::error_code ec;
    tcp::socket socket{context};
    acceptor.accept(socket, ec);

    agent.StartGame(socket, false);
};

void StartClient(const SeabattleField& field, const std::string& ip_str, unsigned short port)
{
    SeabattleAgent agent(field);
    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(ip_str, ec), port);

    if (ec) {
        std::cout << "Wrong IP format"sv << std::endl;
        return;
    }

    net::io_context io_context;
    tcp::socket socket{io_context};
    socket.connect(endpoint, ec);

    if (ec)
    {
        std::cout << "Can't connect to server"sv << std::endl;
        return ;
    }
    agent.StartGame(socket, true);
};

int main(int argc, const char** argv) {
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: program <seed> [<ip>] <port>" << std::endl;
        return 1;
    }

    std::mt19937 engine(std::stoi(argv[1]));
    SeabattleField fieldL = SeabattleField::GetRandomField(engine);

    if (argc == 3) {
        StartServer(fieldL, std::stoi(argv[2]));
    } else if (argc == 4)
    {
        StartClient(fieldL, argv[2], std::stoi(argv[3]));
    }
}
