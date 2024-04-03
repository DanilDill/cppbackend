#include "json_loader.h"
#include <fstream>
#include <iostream>
#include <tuple>
#include <boost/json.hpp>
//#include <boost/json/src.hpp>
namespace json_loader
{
    /*
      using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;
     */
    using  Id = model::Map::Id;
    using Roads = std::vector<model::Road>;
    using Buildings  = std::vector<model::Building>;
    using Offices = std::vector<model::Office>;

    Buildings serializeBuildings(boost::json::value &map_json)
    {
        auto buildings_json  = map_json.at("buildings").as_array();
        Buildings buildings_vec;
        buildings_vec.reserve(buildings_json.size());
        for (auto& build : buildings_json)
        {
            model::Dimension x = build.at("x").as_int64();
            model::Dimension y = build.at("y").as_int64();
            model::Dimension w = build.at("w").as_int64();
            model::Dimension h = build.at("h").as_int64();
            model::Rectangle rect{{x,y},{w,h}};
            buildings_vec.emplace_back(model::Building(rect));
        }
        return buildings_vec;
    }

    Roads  serializeRoads(boost::json::value &map_json)
    {
        auto roads_json = map_json.at("roads").as_array();
        Roads roads_vec;
        roads_vec.reserve(roads_json.size());
        for (auto& road : roads_json)
        {
            model::Coord x0 = road.at("x0").as_int64();
            model::Coord y0 = road.at("y0").as_int64();

            if (road.as_object().contains("x1"))
            {
                model::Coord x1 = road.at("x1").as_int64();
                roads_vec.push_back(model::Road(model::Road::HORIZONTAL,{x0,y0},x1));

            } else if (road.as_object().contains("y1"))
            {
                model::Coord  y1 = road.at("y1").as_int64();
                roads_vec.push_back(model::Road(model::Road::VERTICAL,{x0,y0},y1));
            }
            else
            {
                throw std::logic_error("invalid road");
            }

        }
        return roads_vec;
    }

    Offices serializeOffices(boost::json::value &map_json)
    {
        auto offices_json = map_json.at("offices").as_array();
        Offices offices_vec;
        for (auto& office: offices_json)
        {
            model::Office::Id id(std::string(office.at("id").as_string()));
            model::Point position{model::Coord(office.at("x").as_int64()),model::Coord (office.at("y").as_int64())};
            model::Offset offset{model::Dimension( office.at("offsetX").as_int64()),model::Dimension (office.at("offsetY").as_int64())};
            offices_vec.emplace_back(model::Office(id,position,offset));
        }
        return offices_vec;
    }
    model::Map serialize_map(boost::json::value &map_json)
{
    Id id_ = Id(std::string(map_json.at("id").as_string()));
    std::string map_name(map_json.at("name").as_string());
    Roads roads_ =  serializeRoads(map_json);
    Buildings  buildings_ = serializeBuildings(map_json);
    Offices  offices = serializeOffices(map_json);
    model::Map map(id_,map_name);

    for (auto& road : roads_)
    {
        map.AddRoad(road);
    }

    for (auto& building : buildings_)
    {
        map.AddBuilding(building);
    }

    for (auto & office: offices)
    {
        map.AddOffice(office);
    }
    return map;
}

model::Game LoadGame(const std::filesystem::path& json_path)
{
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    model::Game game;
    // Открываем файл с JSON данными
    std::ifstream file(json_path);
    if (!file.is_open())
    {
        throw std::runtime_error("cannot open json file");
    }
    std::string jsonStr;
    std::string line;
    while (std::getline(file, line))
    {
        jsonStr.append(line);
    }

    auto json_data = boost::json::parse(jsonStr);
    auto maps = json_data.as_object().at("maps").as_array();
    for (auto& map_json : maps)
    {
        auto map = serialize_map(map_json);
        game.AddMap(map);
    }
    return game;
}

}  // namespace json_loader
