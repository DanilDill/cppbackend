#include "player.h"

namespace model
{
    void  Player::move(std::chrono::milliseconds ms)
    {

        Pointf estimated_position =
                {
                        _dog->_coord.x + _dog->_speed.x * std::chrono::duration<double>(ms).count(),
                        _dog->_coord.y + _dog->_speed.y * std::chrono::duration<double>(ms).count()
                };

        auto new_position = bounded_move(estimated_position);
        if (new_position)
        {
            _dog->_coord = *new_position;
            if (new_position->x !=  estimated_position.x || new_position->y != estimated_position.y)
            {
                _dog->_speed = {0.0,0.0};
            }
        }
    }

    std::optional<Pointf> Player::bounded_move(Pointf pointf)
    {
        std::vector<Road> lst;
        for (auto road : _map->GetRoads())
        {
            if (road.IsOnTheRoad(_dog->_coord))
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
        double max_distance = Pointf::measure_distance(_dog->_coord, most_far);
        for (int i = 1 ; i < lst.size();++i)
        {
            Pointf pretender = lst[i].bound_to_the_road(pointf);
            double distanse = Pointf::measure_distance(_dog->_coord,pretender);
            if (distanse > max_distance)
            {
                most_far = pretender;
                max_distance = distanse;
            }
        }
        return most_far;
    }

    void Player::SetSpeed(Direction direction)
    {
        boost::asio::post(_map->getStrand(),[dog = _dog, speed = _map->getDogSpeed(),direction]()
        {
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
    std::shared_ptr<Dog> Player::GetDog() const
    {
        return _dog;
    }
    Map::Id Player::GetMapId()
    {
        return _map->GetId();
    }

    const int Player::GetId() const
    {
        return _id;
    }

    std::string Player::GetName() const
    {
        return  _name;
    }

    void Player::SetRandomized()
    {
        auto roads  = _map->GetRoads();
        size_t n  = roads.size() - 1 ;
        size_t road_num  = std::rand() % n;
        _dog->_coord = { static_cast<double>(roads[road_num].GetStart().x), static_cast<double>(roads[road_num].GetStart().y)};

    }
}