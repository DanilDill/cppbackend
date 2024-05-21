
#include "game_session.h"

namespace model
{
    GameSession::GameSession(boost::asio::io_context&ioc,std::shared_ptr<loot_gen::LootGenerator>loot_gen, std::shared_ptr<Map> map):
    _map(map),_lootGenerator(loot_gen),_strand(boost::asio::make_strand(ioc))
    {
    }
    void GameSession::Tick(std::chrono::milliseconds ms)
    {
        if (!players.empty())
        {
            MovePlayers(ms);
        }
        UpdateLostObjects(ms);

    }
    void GameSession::UpdateLostObjects(std::chrono::milliseconds ms)
    {
        unsigned new_count = _lootGenerator->Generate(ms,_lootCount,players.size());
        if (new_count == _lootCount)
        {
            return;
        }
        _lootCount = new_count;
        _lostObjects = GenerateLostObjects();
    }

    GameSession::LostObjects GameSession::GenerateLostObjects()
    {
        LostObjects new_objects;
        new_objects.reserve(_lootCount);
        size_t loot_types = (_map->GetLootTypes().size()) - 1;
        size_t road_count = _map->GetRoads().size()-1;
        for (int i = 0; i < _lootCount; ++i)
        {
           size_t type = std::rand() % loot_types;
           size_t road_num = std::rand() % road_count;
           Pointf coord = { static_cast<double>(_map->GetRoads()[road_num].GetStart().x), static_cast<double>(_map->GetRoads()[road_num].GetStart().y)};
           new_objects.push_back(std::make_pair(type,coord));
        }
        return new_objects;
    }
    const GameSession::LostObjects& GameSession::GetLostObjects()
    {
        return  _lostObjects;
    }
    void GameSession::SetSpeed(Token token, Direction direction)
    {
        boost::asio::post(_strand,[player = players[token], speed = _map->getDogSpeed(),direction]()
        {
            auto dog = player->GetDog();
            switch (direction)
            {
                case Direction::NORTH:
                    dog->_speed = {0, speed*(-1)};
                    dog->_direction = Direction::NORTH;
                    break;
                case Direction::SOUTH:
                    dog->_speed = {0, speed};
                    dog->_direction = Direction::SOUTH;
                    break;
                case Direction::WEST:
                    dog->_speed = {speed*(-1), 0.0};
                    dog->_direction = Direction::WEST;
                    break;
                case Direction::EAST:
                    dog->_speed = {speed, 0.0};
                    dog->_direction = Direction::EAST;
                    break;
                case Direction::STOP:
                    dog->_speed = {0.0, 0.0};
                    dog->_direction = Direction::STOP;
                    break;
            }
        });
    }

    void GameSession::AddPlayer(Token token, std::shared_ptr<Player> player)
    {
        players.try_emplace(token,player);
    }
    void GameSession::MovePlayers(std::chrono::milliseconds ms)
    {
        for (auto player: players)
        {
            MovePlayer(player.second,_map,ms);
        }
    }


    std::optional<Pointf> GameSession::bounded_move(std::shared_ptr<Dog> dog, std::shared_ptr<Map> map,Pointf pointf)
    {
        std::vector<Road> lst;
        for (auto road : map->GetRoads())
        {
            if (road.IsOnTheRoad(dog->_coord))
            {
                lst.emplace_back(std::move(road));
            }
        }
        if (lst.size() == 0)
        {
            return std::nullopt;
        }
        Pointf most_far = lst[0].bound_to_the_road(pointf);
        if (lst.size() == 1)
        {
            return most_far;
        }
        double max_distance = Pointf::measure_distance(dog->_coord, most_far);
        for (int i = 1 ; i < lst.size();++i)
        {
            Pointf pretender = lst[i].bound_to_the_road(pointf);
            double distanse = Pointf::measure_distance(dog->_coord,pretender);
            if (distanse > max_distance)
            {
                most_far = pretender;
                max_distance = distanse;
            }
        }
        return most_far;
    }

    std::optional<std::shared_ptr<Player>> GameSession::FindPlayer(Token token)
    {
        auto player = players.find(token);
        if (player != players.end())
        {
            return player->second;
        }
        return std::nullopt;
    }
    void GameSession::MovePlayer(std::shared_ptr<Player> player, std::shared_ptr<Map> map, std::chrono::milliseconds ms)
    {
        auto dog = player->GetDog();
        Pointf estimated_position =
                {
                        dog->_coord.x + dog->_speed.x * std::chrono::duration<double>(ms).count(),
                        dog->_coord.y + dog->_speed.y * std::chrono::duration<double>(ms).count()
                };

        auto new_position = bounded_move(dog,map,estimated_position);
        if (new_position)
        {
            dog->_coord = *new_position;
            if (new_position->x !=  estimated_position.x || new_position->y != estimated_position.y)
            {
                dog->_speed = {0.0,0.0};
            }
        }
    }
}