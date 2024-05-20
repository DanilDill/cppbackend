
#include "game_session.h"

namespace model
{
    GameSession::GameSession(std::shared_ptr<Map> map):
    _map(map)
    {
    }
    void GameSession::Tick(std::chrono::milliseconds ms)
    {
        if (!_players.empty())
        {
            MovePlayers(ms);
        }

    }
    void GameSession::AddPlayer(std::shared_ptr<Player> player)
    {
        _players.push_back(player);
    }
    void GameSession::MovePlayers(std::chrono::milliseconds ms)
    {
        for (auto player:_players)
        {
            MovePlayer(player,_map,ms);
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