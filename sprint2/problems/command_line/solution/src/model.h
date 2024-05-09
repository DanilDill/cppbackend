#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <atomic>
#include <memory>
#include <optional>
#include "tokenizer.h"
#include "tagged.h"
#include "ticker.h"
#include <boost/asio.hpp>

namespace model {

using Dimension = int;
using Coord = Dimension;
using Coordf = double;
struct Point {
    Coord x, y;
};
struct Pointf
{
   Coordf x, y;
    bool operator<=(const Pointf& other) const{ return x <= other.x and y<= other.y;};
    static double  measure_distance(const Pointf& first, const Pointf second)
    {
        double squared_distance = pow(second.x - first.x, 2) + pow(second.y - first.y, 2);
        return sqrt(squared_distance);
    }
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
        , end_{end_x, start.y}
        {
            makeCorners();
        }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y}
        {
            makeCorners();
        }
    bool IsOnTheRoad(Pointf point) const
    {
        return left_bottom_corner <= point && point <= right_top_corner;
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
    Pointf bound_to_the_road(Pointf point)
    {
        double new_x = bound(left_bottom_corner.x, right_top_corner.x, point.x);
        double new_y = bound(left_bottom_corner.y, right_top_corner.y, point.y);
        return Pointf{new_x, new_y};
    }
    static double bound(double bound_1, double bound_2,double value)
    {
        double lower = std::min(bound_1, bound_2);
        double upper = std::max(bound_1, bound_2);

        double result = std::min(upper, value);
        result = std::max(lower, result);
        return result;
    }
private:
    void makeCorners()
    {
        double left_bottom_x = std::min(start_.x,end_.x) - width;
        double left_bottom_y = std::min(start_.y, end_.y) - width;
        double right_top_x = std::max(start_.x, end_.x) + width;
        double right_top_y = std::max(start_.y, end_.y) + width;
        left_bottom_corner = {left_bottom_x, left_bottom_y};
        right_top_corner = {right_top_x, right_top_y};
    }
    Pointf left_bottom_corner;
    Pointf right_top_corner;
    const double width = 0.4;
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
    using
    Id = util::Tagged<std::string, Map>;
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
    void  move(std::chrono::milliseconds ms)
    {

        Pointf estimated_position = {
                _dog->_coord.x + _dog->_speed.x * std::chrono::duration<double>(ms).count(),
                _dog->_coord.y + _dog->_speed.y * std::chrono::duration<double>(ms).count()
        };

        auto new_position = bounded_move(estimated_position);
        if (new_position)
        {
            _dog->_coord = *new_position;
            if (new_position->x !=  estimated_position.x || new_position->y != estimated_position.y)
            {
             //   _dog->_direction = Direction::STOP;
                _dog->_speed = {0.0,0.0};
            }
        }
    }
    std::optional<Pointf> bounded_move(Pointf pointf)
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

    void SetSpeed(Direction direction)
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

    void SetRandomized()
    {
        auto roads  = _map->GetRoads();
        size_t n  = roads.size() - 1 ;
        size_t road_num  = std::rand() % n;
        _dog->_coord = { static_cast<double>(roads[road_num].GetStart().x), static_cast<double>(roads[road_num].GetStart().y)};

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
    void SetRandomizedCoord()
    {
        randomized_coord = true;
    }
    void Tick(std::chrono::milliseconds  ms);
    bool hasTicker()
    {
        return ticker == nullptr;
    }
    void SetTicker(std::chrono::milliseconds ms )
    {
        auto api_strand = net::make_strand(ioc);
        ticker = std::make_shared<Ticker>(api_strand,ms,
                                          [this](std::chrono::milliseconds delta) { Tick(delta);});
        ticker->Start();
    }
    void SetDefaultDogSpeed(double speed);
    double GetDefaultDogSpeed()const;
    void AddMap(Map map);
    int AddPlayer(Token t, const std::string& player_name, const Map::Id& id);
    const Maps& GetMaps() const noexcept;
    std::optional<Player> FindPlayer(Token t) const;
    const Players& GetPLayers();
    const std::shared_ptr<Map> FindMap(const Map::Id& id) const noexcept;
    void SetDirection(Token t, Direction direction)
    {
        FindPlayer(t)->SetSpeed(direction);
    }
private:
    std::shared_ptr<Ticker> ticker = nullptr;
    bool randomized_coord = false;
    boost::asio::io_context& ioc;
    Players players;
    Maps maps_;
    MapIdToIndex map_id_to_index_;
    double default_dogspeed;
};

}  // namespace model
