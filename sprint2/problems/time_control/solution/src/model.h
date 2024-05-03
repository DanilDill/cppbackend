#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <atomic>
#include <memory>
#include <optional>
#include "Tokenizer.h"
#include "tagged.h"

#include <boost/asio.hpp>

namespace model {

using Dimension = int;
using Coord = Dimension;
using Coordf = float;
struct Point {
    Coord x, y;
};
struct Pointf
{
   Coordf x, y;
};
struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

enum  Direction
{
    NORTH ,
    SOUTH,
    WEST ,
    EAST,
    STOP,
    UNKNOWN
};
std::string to_string(Direction direction);
Direction to_direction(const std::string&);
class Road {
    struct HorizontalTag {
        explicit HorizontalTag() = default;
    };

    struct VerticalTag {
        explicit VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}
        , end_{end_x, start.y} {
    }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y} {
    }

    bool IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point GetStart() const noexcept {
        return start_;
    }

    Point GetEnd() const noexcept {
        return end_;
    }

private:
    Point start_;
    Point end_;
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}
        , position_{position}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Point position_;
    Offset offset_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;

    Map(Id id, std::string name,boost::asio::io_context&ioContext) noexcept
        : id_(std::move(id))
        , name_(std::move(name)),
        _strand(boost::asio::make_strand(ioContext)){
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const Buildings& GetBuildings() const noexcept {
        return buildings_;
    }

    const Roads& GetRoads() const noexcept {
        return roads_;
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    void AddRoad(const Road& road) {
        roads_.emplace_back(road);
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddOffice(Office office);

    void setDogSpeed(double speed);
    double getDogSpeed()const;
    boost::asio::strand<boost::asio::io_context::executor_type>& getStrand()
    {
        return _strand;
    }
private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;
    boost::asio::strand<boost::asio::io_context::executor_type> _strand;
    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;
    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
    double dogspeed;
};
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
    void  move(size_t ms)
    {
        _map->GetRoads();
        _dog->_coord.x += _dog->_speed.x * (static_cast<double>(ms) / 1000);
        _dog->_coord.y += _dog->_speed.y * (static_cast<double>(ms) / 1000);

    }
    std::shared_ptr<Dog> GetDog() const
    {
        return _dog;
    }
    Map::Id GetMapId()
    {
        return _map->GetId();
    }

    const int GetId() const
   {
       return _id;
   }

    std::string GetName() const
    {
        return  _name;
    }

};


class Game {
public:
    using Maps = std::vector<std::shared_ptr<Map>>;
    using PlayerHasher = util::TaggedHasher<Token>;
    using Players = std::unordered_map<Token,Player,PlayerHasher>;
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
    Game(boost::asio::io_context& context): ioc(context){};
    void Tick(size_t ms);
    void SetDefaultDogSpeed(double speed);
    double GetDefaultDogSpeed()const;
    void AddMap(Map map);
    int AddPlayer(Token t, const std::string& player_name, const Map::Id& id);
    const Maps& GetMaps() const noexcept;
    std::optional<Player> FindPlayer(Token t) const;
    const Players& GetPLayers();
    const std::shared_ptr<Map> FindMap(const Map::Id& id) const noexcept;
    void Move(Token t, Direction direction)
    {

        if (auto it = map_id_to_index_.find( FindPlayer(t)->GetMapId()); it != map_id_to_index_.end())
        {
            boost::asio::post(maps_.at(it->second)->getStrand(),[this,t,direction,speed = maps_.at(it->second)->getDogSpeed()]()
            {
                switch (direction)
                {
                    case Direction::NORTH:
                        players[t].GetDog()->_speed = {0, speed*(-1)};
                        break;
                    case Direction::SOUTH:
                        players[t].GetDog()->_speed = {0, speed};
                        break;
                    case Direction::WEST:
                        players[t].GetDog()->_speed = {speed*(-1), 0};
                        break;
                    case Direction::EAST:
                        players[t].GetDog()->_speed = {speed, 0};
                        break;
                    case Direction::STOP:
                        players[t].GetDog()->_speed = {0, 0};
                        break;
                }
            });
        }
    }
private:
    boost::asio::io_context& ioc;
    Players players;
    Maps maps_;
    MapIdToIndex map_id_to_index_;
    double default_dogspeed;
};

}  // namespace model
