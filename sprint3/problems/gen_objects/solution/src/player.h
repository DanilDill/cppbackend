#pragma once
#include <chrono>
#include <memory>
#include <optional>
#include "point.h"
#include "direction.h"
#include "game_map.h"
namespace model
{
    struct DogSpeed
    {
        double x;
        double y;
    };
    struct Dog
    {

        Pointf _coord;
        DogSpeed _speed = {0.0,0.0};
        Direction _direction = Direction::NORTH;
        Dog()=default;
    };
    class Player
    {
    private:
        int _id;
        std::shared_ptr<Map> _map;
        std::string _name;
        std::shared_ptr<Dog> _dog= std::make_shared<Dog>();
    public:
        Player(){};
        Player(int id,const std::string& name, std::shared_ptr<Map> map):
                _name(name),_map(map),_id(id)
        {
        };
        void  move(std::chrono::milliseconds ms);
        std::optional<Pointf> bounded_move(Pointf pointf);
        void SetSpeed(Direction direction);

        void SetRandomized();
        std::shared_ptr<Dog> GetDog() const;

        Map::Id GetMapId();
        const int GetId() const;
        std::string GetName() const;

    };

}