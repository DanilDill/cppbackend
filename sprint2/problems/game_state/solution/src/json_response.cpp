#define BOOST_JSON_STANDALONE
#include "json_response.h"
#include "json_atribute.h"
#include <boost/json.hpp>
namespace json_responce
{
    using respAttr = JsonAttribute::ResponseAtrrubutes;

    std::string to_json(const std::vector<model::Map>& maps)
    {
        using attr = JsonAttribute::MapArrayAttributes::MapAttributes;
        boost::json::array maps_json;
        for (const auto& map : maps)
        {
            boost::json::object object;
            object[attr::ATTR_ID] = *map.GetId();
            object[attr::ATTR_NAME] = map.GetName();
            maps_json.emplace_back(object);
        }
        return boost::json::serialize(maps_json);
    }

    std::string AuthTokenJson(const std::string& token_str, int id)
    {
        boost::json::object response_json;
        response_json["authToken"] = token_str;
        response_json["playerId"] = id;
        return  boost::json::serialize(response_json);
    }
    std::string ErrorJson(const std::string& code, const std::string& message)
    {
        boost::json::object response_json;
        response_json[respAttr::CODE] = code;
        response_json[respAttr::MESSAGE] = message;
        return  boost::json::serialize(response_json);
    }

    boost::json::array to_json(const model::Map::Roads& roads_vec)
    {
        boost::json::array roads_json;
        using attr = JsonAttribute::RoadArrayAttributes;
        for (const auto& road: roads_vec)
        {
            boost::json::object road_json;
            model::Point start = road.GetStart();
            road_json[attr::ATTR_X0] = start.x;
            road_json[attr::ATTR_Y0] = start.y;
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
        using attr = JsonAttribute::BuildArrayAttributes;
        for (const auto building: buildings_vec)
        {
            boost::json::object building_json;
            auto bounds = building.GetBounds();
            building_json[attr::ATTR_X] = bounds.position.x;
            building_json[attr::ATTR_Y] = bounds.position.y;
            building_json[attr::ATTR_WIDTH] = bounds.size.width;
            building_json[attr::ATTR_HEIGHT] = bounds.size.height;
            buildings_json.emplace_back(building_json);
        }
        return buildings_json;
    }

    boost::json::array to_json(const model::Map::Offices& offices_vec)
    {
        boost::json::array offices_json;
        using attr = JsonAttribute::OfficeArrayAttributes;
        for (const auto& office : offices_vec)
        {
            boost::json::object office_json;
            office_json[attr::ATTR_ID] = *office.GetId();
            office_json[attr::ATTR_X] = office.GetPosition().x;
            office_json[attr::ATTR_Y] = office.GetPosition().y;
            office_json[attr::ATTR_OFFSET_X] = office.GetOffset().dx;
            office_json[attr::ATTR_OFFSET_Y] = office.GetOffset().dy;
            offices_json.emplace_back(office_json);
        }
        return offices_json;
    }
    std::string to_json(const model::Map& map)
    {
        boost::json::object map_json;
        using attr = JsonAttribute::MapArrayAttributes::MapAttributes;
        map_json[attr::ATTR_ID] = *map.GetId();
        map_json[attr::ATTR_NAME] = map.GetName();
        map_json[attr::ATTR_ROADS] = to_json(map.GetRoads());
        map_json[attr::ATTR_BUILDINGS] = to_json(map.GetBuildings());
        map_json[attr::ATTR_OFFICES] = to_json(map.GetOffices());
        return boost::json::serialize(map_json);
    }
}