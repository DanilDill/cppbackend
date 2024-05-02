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
    EAST
};

std::string to_string(Direction direction);
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

    Map(Id id, std::string name) noexcept
        : id_(std::move(id))
        , name_(std::move(name)) {
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

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
};

struct Dog
{

    Pointf _coord;
    float _speed = 0.0;
    Direction _direction = Direction::NORTH;
    Dog()=default;
};
class Player
{
private:
    int _id;
    Map::Id _map_id;
    std::string _name;
    std::shared_ptr<Dog> _dog= std::make_shared<Dog>();
public:
    Player():_map_id(""){};
    Player(int id,const std::string& name,const Map::Id& map_id):
    _name(name),_map_id(map_id),_id(id)
    {};

    std::shared_ptr<Dog> GetDog() const
    {
        return _dog;
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
    using Maps = std::vector<Map>;
    using PlayerHasher = util::TaggedHasher<Token>;
    using Players = std::unordered_map<Token,Player,PlayerHasher>;
    void AddMap(Map map);
    int AddPlayer(Token t, const std::string& player_name, const Map::Id& id)
    {
        auto size = players.size();
        players[t] = Player(size,player_name,id);
        return players[t].GetId();
    };
    const Maps& GetMaps() const noexcept {
        return maps_;
    }
    std::optional<Player> FindPlayer(Token t) const
    {
         auto player = players.find(t);
        if (player != players.end())
        {
            return player->second;
        }
        return std::nullopt;
    }

    const Players& GetPLayers()
    {
        return players;
    }

    const Map* FindMap(const Map::Id& id) const noexcept {
        if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
            return &maps_.at(it->second);
        }
        return nullptr;
    }

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;

    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

    Players players;
    std::vector<Map> maps_;
    MapIdToIndex map_id_to_index_;
};

}  // namespace model
