#pragma once
#include <memory>
#include <chrono>
#include "player.h"
#include "map"
#include "tokenizer.h"
#include "loot_generator.h"
namespace model
{
    class GameSession
    {
    public:
            using PlayerHasher = util::TaggedHasher<Token>;
            using Players = std::unordered_map<Token,std::shared_ptr<Player>,PlayerHasher>;
            using LostObjects =  std::vector<std::pair<size_t,Pointf>>;
            GameSession(boost::asio::io_context&ioc,std::shared_ptr<loot_gen::LootGenerator> lootGenerator,std::shared_ptr<Map> map);
            void Tick(std::chrono::milliseconds ms);
            void AddPlayer(Token token, std::shared_ptr<Player> player);
            std::optional<std::shared_ptr<Player>> FindPlayer(Token token);
            void SetSpeed(Token token, Direction direction);
            const LostObjects& GetLostObjects();
        private:
            void UpdateLostObjects(std::chrono::milliseconds ms);
            void GenerateLostObjects();
            void MovePlayers(std::chrono::milliseconds ms);
            static void  MovePlayer(std::shared_ptr<Player>,std::shared_ptr<Map> map,std::chrono::milliseconds ms);
            static std::optional<Pointf> bounded_move(std::shared_ptr<Dog>dog, std::shared_ptr<Map> map, Pointf pointf);
        private:
            LostObjects _lostObjects;
            std::shared_ptr<loot_gen::LootGenerator> _lootGenerator = nullptr;
            boost::asio::strand<boost::asio::io_context::executor_type> _strand;
            unsigned _lootCount = 0;
            Players players;
            std::shared_ptr<Map>_map;
    };
}

