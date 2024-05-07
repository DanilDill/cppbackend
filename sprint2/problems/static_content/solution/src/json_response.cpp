#include "json_response.h"
#include <boost/json.hpp>
namespace json_responce
{
    std::string to_json(const std::vector<model::Map>& maps)
    {
        boost::json::array maps_json;
        for (const auto& map : maps)
        {
            boost::json::object object;
            object["id"] = *map.GetId();
            object["name"] = map.GetName();
            maps_json.emplace_back(object);
        }
        return boost::json::serialize(maps_json);
    }

    std::string BadRequestJson()
    {
        boost::json::object response_json;
        response_json["code"] = "badRequest";
        response_json["message"] = "Bad request";
        return  boost::json::serialize(response_json);
    }

    std::string NotFoundJson()
    {
        boost::json::object response_json;
        response_json["code"] = "mapNotFound";
        response_json["message"] = "Map not found";
        return boost::json::serialize(response_json);
    }
    boost::json::array to_json(const model::Map::Roads& roads_vec)
    {
        boost::json::array roads_json;
        for (const auto& road: roads_vec)
        {
            boost::json::object road_json;
            model::Point start = road.GetStart();
            road_json["x0"] = start.x;
            road_json["y0"] = start.y;
            model::Point end = road.GetEnd();
            if (road.IsHorizontal())
            {
               road_json["x1"] = end.x;
            } else
            {
                road_json["y1"] = end.y;
            }
            roads_json.emplace_back(road_json);
        }
        return roads_json;
    }


    boost::json::array to_json(const model::Map::Buildings& buildings_vec)
    {
        boost::json::array buildings_json;
        for (const auto building: buildings_vec)
        {
            boost::json::object building_json;
            auto bounds = building.GetBounds();
            building_json["x"] = bounds.position.x;
            building_json["y"] = bounds.position.y;
            building_json["w"] = bounds.size.width;
            building_json["h"] = bounds.size.height;
            buildings_json.emplace_back(building_json);
        }
        return buildings_json;
    }

    boost::json::array to_json(const model::Map::Offices& offices_vec)
    {
        boost::json::array offices_json;
        for (const auto& office : offices_vec)
        {
            boost::json::object office_json;
            office_json["id"] = *office.GetId();
            office_json["x"] = office.GetPosition().x;
            office_json["y"] = office.GetPosition().y;
            office_json["offsetX"] = office.GetOffset().dx;
            office_json["offsetY"] = office.GetOffset().dy;
            offices_json.emplace_back(office_json);
        }
        return offices_json;
    }
    std::string to_json(const model::Map& map)
    {
        boost::json::object map_json;
        map_json["id"] = *map.GetId();
        map_json["name"] = map.GetName();
        map_json["roads"] = to_json(map.GetRoads());
        map_json["buildings"] = to_json(map.GetBuildings());
        map_json["offices"] = to_json(map.GetOffices());
        return boost::json::serialize(map_json);
    }
}