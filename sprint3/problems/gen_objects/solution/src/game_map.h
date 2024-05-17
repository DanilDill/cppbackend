#pragma once
#include <string>
#include "road.h"
#include "building.h"
#include "tagged.h"
#include "office.h"
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
        Map(Id id, std::string name,boost::asio::io_context&ioContext) noexcept;
        const Id& GetId() const noexcept;
        const std::string& GetName() const noexcept;
        const Buildings& GetBuildings() const noexcept;
        const Roads& GetRoads() const noexcept;
        const Offices& GetOffices() const noexcept;
        void AddRoad(const Road& road);
        void AddBuilding(const Building& building);
        void AddOffice(Office office);
        void setDogSpeed(double speed);
        double getDogSpeed()const;
        boost::asio::strand<boost::asio::io_context::executor_type>& getStrand();
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

}