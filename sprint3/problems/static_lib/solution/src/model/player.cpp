#include "player.h"

namespace model
{


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