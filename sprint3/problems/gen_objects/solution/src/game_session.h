#pragma once
#include <memory>
#include <chrono>
#include "player.h"
#include "map"
namespace model
{
    class GameSession
    {
    public:
            GameSession(std::shared_ptr<Map> map);
            void Tick(std::chrono::milliseconds ms);
            void AddPlayer(std::shared_ptr<Player>);

        private:
            void MovePlayers(std::chrono::milliseconds ms);
            static void  MovePlayer(std::shared_ptr<Player>,std::chrono::milliseconds ms);
            std::optional<Pointf> bounded_move(std::shared_ptr<Dog>dog, Pointf pointf);
        private:
            std::vector<std::shared_ptr<Player>>_players;
            std::shared_ptr<Map>_map;
    };
}

