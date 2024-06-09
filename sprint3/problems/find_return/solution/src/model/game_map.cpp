#include "game_map.h"

namespace model
{
    Map::Map(Id id, std::string name,boost::asio::io_context& ioContext) noexcept
    : id_(std::move(id))
    , name_(std::move(name))
    {
    }

    const Map::Id& Map::GetId() const noexcept {
        return id_;
    }

    const std::string& Map::GetName() const noexcept {
        return name_;
    }

    const Map::Buildings& Map::GetBuildings() const noexcept {
        return buildings_;
    }

    const Map::Roads& Map::GetRoads() const noexcept {
        return roads_;
    }

    const Map::Offices& Map::GetOffices() const noexcept {
        return offices_;
    }

    void Map::AddRoad(const Road& road) {
        roads_.emplace_back(road);
    }

    void Map::AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    const Map::LootTypes& Map::GetLootTypes() const noexcept
    {
        return _loot_types;
    }

    void Map::AddOffice(Office office) {
        if (warehouse_id_to_index_.contains(office.GetId())) {
            throw std::invalid_argument("Duplicate warehouse");
        }

        const size_t index = offices_.size();
        Office& o = offices_.emplace_back(std::move(office));
        try {
            warehouse_id_to_index_.emplace(o.GetId(), index);
        } catch (...) {
            offices_.pop_back();
            throw;
        }
    }

    void Map::AddLootType(const LootType& lootType)
    {
        _loot_types.push_back(lootType);
    }

    void Map::setDogSpeed(double speed)
    {
        dogspeed = speed;
    }
    double Map::getDogSpeed()const
    {
        return dogspeed;
    }

    void Map::setDefaultBagCapacity(size_t capacity)
    {
        defaultBagCapacity = capacity;
    }
}