#pragma once
#include <memory>
#include <chrono>
#include "player.h"
#include "map"
#include "tokenizer.h"
#include "loot_generator.h"
#include "collision_detector.h"
namespace model
{
    class GameSession
    {
    public:
        struct GameItem : public collision_detector::Item
        {
            size_t type;
            GameItem() : collision_detector::Item()
            {
                width = 0;
            }
            GameItem(size_t type_num,geom::Point2D point): collision_detector::Item()
            {
                type = type_num;
                width = 0;
                position = point;
            }
        };

    private:
        struct GameGatherer : collision_detector::Gatherer
        {
            GameGatherer(const Player& player_link) : collision_detector::Gatherer(),player(player_link)
            {
                width = 0.6;
            }
            const Player& player;
        };
        struct BuildingItem : public collision_detector::Item
        {

        };
        class ItemProvider : public collision_detector::ItemGathererProvider
        {
        private:
            std::vector<collision_detector::Item>_items;
            std::vector<collision_detector::Gatherer> _gatherers;
        public:
            ItemProvider(const std::vector<collision_detector::Item>& items, std::vector<collision_detector::Gatherer>& gatherers):
                    _items(items), _gatherers(gatherers)
            {
            };
            size_t ItemsCount() const override{return _items.size();};
            collision_detector::Item GetItem(size_t idx) const override{ return _items.at(idx);};
            size_t GatherersCount() const override{return  _gatherers.size();};
            collision_detector::Gatherer GetGatherer(size_t idx) const override{return _gatherers.at(idx);};
        };
    public:
            using PlayerHasher = util::TaggedHasher<Token>;
            using Players = std::unordered_map<Token,std::shared_ptr<Player>,PlayerHasher>;
            using LostObject = GameItem;
            using LostObjects =  std::vector<LostObject>;
            GameSession(boost::asio::io_context&ioc,std::shared_ptr<loot_gen::LootGenerator> lootGenerator,std::shared_ptr<Map> map);
            void Tick(std::chrono::milliseconds ms);
            void AddPlayer(Token token, std::shared_ptr<Player> player);
            std::optional<std::shared_ptr<Player>> FindPlayer(Token token);
            void SetSpeed(Token token, Direction direction);
            const LostObjects& GetLostObjects();
        private:
            void CheckCollision();
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
            Players _players;
            std::shared_ptr<Map>_map;
    };
}

