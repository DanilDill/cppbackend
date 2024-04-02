#include "json_loader.h"
#include <fstream>
#include <iostream>
#include <tuple>
#include "boost_json.cpp"
#include <boost/json/src.hpp>
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

            } else if (road.as_object().contains("x1"))
            {

            }
            else
            {
                throw std::logic_error("invalid road");
            }

          //  model::Coord x1 = build.at("w").as_int64();
         //   model::Coord h = build.at("h").as_int64();
        }
    }
    model::Map serialize_map(boost::json::value &map_json)
{
    Id id_ = Id(std::string(map_json.at("id").as_string()));
    std::string map_name(map_json.at("name").as_string());
    Roads roads_ ;

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
    for (auto& map : maps)
    {
        std::cout << boost::json::serialize(map)<<std::endl;
    }
    return game;
}

}  // namespace json_loader
