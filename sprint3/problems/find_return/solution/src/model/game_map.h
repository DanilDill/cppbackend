#pragma once
#include <string>
#include "road.h"
#include "building.h"
#include "tagged.h"
#include "office.h"
#include "loot_type.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>

namespace model
{
    class Map {
    public:
        using
        Id = util::Tagged<std::string, Map>;
        using Roads = std::vector<Road>;
        using Buildings = std::vector<Building>;
        using Offices = std::vector<Office>;
        using LootTypes =  std::vector<model::LootType>;
        Map(Id id, std::string name,boost::asio::io_context&ioContext) noexcept;
        const Id& GetId() const noexcept;
        const std::string& GetName() const noexcept;
        const Buildings& GetBuildings() const noexcept;
        const Roads& GetRoads() const noexcept;
        const Offices& GetOffices() const noexcept;
        const LootTypes & GetLootTypes() const noexcept;
        void AddRoad(const Road& road);
        void AddBuilding(const Building& building);
        void AddOffice(Office office);
        void AddLootType(const LootType& lootType);
        void setDogSpeed(double speed);
        void setDefaultBagCapacity(size_t capacity);
        double getDogSpeed()const;

    private:
        std::vector<model::LootType> _loot_types;
        using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

        Id id_;
        std::string name_;
        Roads roads_;
        Buildings buildings_;
        OfficeIdToIndex warehouse_id_to_index_;
        Offices offices_;
        double dogspeed;
        size_t defaultBagCapacity;
    };

}