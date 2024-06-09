#include "model_loader.h"
#include "json_atribute.h"
#include <fstream>
#include <iostream>
#include <boost/json.hpp>
namespace model
{
    using  Id = model::Map::Id;
    using Roads = std::vector<model::Road>;
    using Buildings  = std::vector<model::Building>;
    using Offices = std::vector<model::Office>;
    using LootTypes = std::vector<model::LootType>;
    Buildings serializeBuildings(const boost::json::value &map_json)
    {
        using attributes = JsonAttribute::BuildArrayAttributes;
        auto buildings_json  = map_json.at(attributes::NAME).as_array();
        Buildings buildings_vec;
        buildings_vec.reserve(buildings_json.size());
        for (const auto& build : buildings_json)
        {
            model::Dimension x = build.at(attributes::ATTR_X).as_int64();
            model::Dimension y = build.at(attributes::ATTR_Y).as_int64();
            model::Dimension w = build.at(attributes::ATTR_WIDTH).as_int64();
            model::Dimension h = build.at(attributes::ATTR_HEIGHT).as_int64();
            model::Rectangle rect{{x,y},{w,h}};
            buildings_vec.emplace_back(model::Building(rect));
        }
        return buildings_vec;
    }

    Roads  serializeRoads(const boost::json::value &map_json)
    {
        using attributes = JsonAttribute::RoadArrayAttributes;
        auto roads_json = map_json.at(attributes::NAME).as_array();
        Roads roads_vec;
        roads_vec.reserve(roads_json.size());
        for (auto& road : roads_json)
        {
            model::Coord x0 = road.at(attributes::ATTR_X0).as_int64();
            model::Coord y0 = road.at(attributes::ATTR_Y0).as_int64();

            if (road.as_object().contains(attributes::ATTR_X1))
            {
                model::Coord x1 = road.at(attributes::ATTR_X1).as_int64();
                roads_vec.push_back(model::Road(model::Road::HORIZONTAL,{x0,y0},x1));

            } else if (road.as_object().contains(attributes::ATTR_Y1))
            {
                model::Coord  y1 = road.at(attributes::ATTR_Y1).as_int64();
                roads_vec.push_back(model::Road(model::Road::VERTICAL,{x0,y0},y1));
            }
            else
            {
                throw std::logic_error("invalid road");
            }

        }
        return roads_vec;
    }

    Offices  serializeOffices(const boost::json::value &map_json)
    {
        using attribute = JsonAttribute::OfficeArrayAttributes;
        auto offices_json = map_json.at(attribute::NAME).as_array();
        Offices offices_vec;
        for (auto& office: offices_json)
        {
            model::Office::Id id(std::string(office.at(attribute::ATTR_ID).as_string()));
            model::Point position{model::Coord(office.at(attribute::ATTR_X).as_int64()),model::Coord (office.at(attribute::ATTR_Y).as_int64())};
            model::Offset offset{model::Dimension( office.at(attribute::ATTR_OFFSET_X).as_int64()),model::Dimension (office.at(attribute::ATTR_OFFSET_Y).as_int64())};
            offices_vec.emplace_back(model::Office(id,position,offset));
        }
        return offices_vec;
    }

    LootTypes serializeLootTypes(const boost::json::value &map_json)
    {
        using attribute = JsonAttribute::LootTypesArr;
        auto lootTypesJson = map_json.at(attribute::NAME).as_array();
        LootTypes result;
        result.reserve(lootTypesJson.size());
        for (const auto&  lootType: lootTypesJson)
        {
            model::LootType lootTypeMap;
            if(lootType.as_object().contains(attribute::LootType::ATTR_NAME))
            {
                lootTypeMap[attribute::LootType::ATTR_NAME] = std::string(lootType.as_object().at(attribute::LootType::ATTR_NAME).as_string().c_str());
            }
            if(lootType.as_object().contains(attribute::LootType::ATTR_COLOR))
            {
                lootTypeMap[attribute::LootType::ATTR_COLOR] = std::string(lootType.as_object().at(attribute::LootType::ATTR_COLOR).as_string().c_str());
            }
            if(lootType.as_object().contains(attribute::LootType::ATTR_FILE))
            {
                lootTypeMap[attribute::LootType::ATTR_FILE] = std::string(lootType.as_object().at(attribute::LootType::ATTR_FILE).as_string().c_str());
            }
            if(lootType.as_object().contains(attribute::LootType::ATTR_ROTATION))
            {
                lootTypeMap[attribute::LootType::ATTR_ROTATION] = lootType.as_object().at(attribute::LootType::ATTR_ROTATION).as_int64();
            }
            if(lootType.as_object().contains(attribute::LootType::ATTR_SCALE))
            {
                lootTypeMap[attribute::LootType::ATTR_SCALE] = lootType.as_object().at(attribute::LootType::ATTR_SCALE).as_double();
            }
            if(lootType.as_object().contains(attribute::LootType::ATTR_TYPE))
            {
                lootTypeMap[attribute::LootType::ATTR_TYPE] = std::string(lootType.as_object().at(attribute::LootType::ATTR_TYPE).as_string().c_str());
            }
            result.push_back(lootTypeMap);
        }
        return result;
    }
    model::Map serialize_map(const boost::json::value &map_json, double speed,size_t defaultBagCapacity, boost::asio::io_context& ioContext)
{
    using attributes = JsonAttribute::MapArrayAttributes::MapAttributes;
    Id id_ = Id(std::string(map_json.at(attributes::ATTR_ID).as_string()));
    std::string map_name(map_json.at(attributes::ATTR_NAME).as_string());
    Roads roads_ =  serializeRoads(map_json);
    Buildings  buildings_ = serializeBuildings(map_json);
    Offices  offices = serializeOffices(map_json);
    LootTypes lootTypes = serializeLootTypes(map_json);
    model::Map map(id_,map_name,ioContext);

    for (const auto& road : roads_)
    {
        map.AddRoad(road);
    }


    for (const auto& building : buildings_)
    {
        map.AddBuilding(building);
    }

    for (const auto & office: offices)
    {
        map.AddOffice(office);
    }

    for (const auto& lootType: lootTypes)
    {
        map.AddLootType(lootType);
    }
    if (auto iter = map_json.as_object().find(JsonAttribute::MapArrayAttributes::MapAttributes::DOG_SPEED);
        iter != map_json.as_object().end())
    {
        map.setDogSpeed(iter->value().as_double());
    }
    else
    {
        map.setDogSpeed(speed);
    }

    if (auto iter = map_json.as_object().find(JsonAttribute::MapArrayAttributes::MapAttributes::BAG_CAPACITY);
            iter != map_json.as_object().end())
    {
        map.setDogSpeed(iter->value().as_int64());
    }
    else
    {
        map.setDogSpeed(defaultBagCapacity);
    }

    return map;
}
    std::shared_ptr<loot_gen::LootGenerator> serializeLootGenerator(const boost::json::value& json_data)
    {
        auto loot_generator_json = json_data.as_object().at(JsonAttribute::LootGeneratorConfig::NAME);
        uint64_t period = loot_generator_json.as_object().at(JsonAttribute::LootGeneratorConfig::ATTR_PERIOD).as_double() * 1000;
        double probability = loot_generator_json.as_object().at(JsonAttribute::LootGeneratorConfig::ATTR_PROBABILITY).as_double();
        return std::make_shared<loot_gen::LootGenerator>(loot_gen::LootGenerator::TimeInterval(period),probability);
    }
model::Game LoadGame(const std::filesystem::path& json_path, boost::asio::io_context& ioc)
{
    model::Game game(ioc);
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

    size_t defaultBagCapacity = 3;
    if (auto iter = json_data.as_object().find(JsonAttribute::DEFAULT_BAG_CAPACITY);iter != json_data.as_object().end())
    {
        defaultBagCapacity =  iter->value().as_int64();
    }
    game.SetDefaultBagCapacity(defaultBagCapacity);

    double dogspeed  = 1.0;
    if (auto  iter = json_data.as_object().find(JsonAttribute::DOG_SPEED_DEFAULT); iter != json_data.as_object().end()  )
    {
        dogspeed = iter->value().as_double();
    }
    game.SetDefaultDogSpeed(dogspeed);

    auto  loot_generator = serializeLootGenerator(json_data);
    game.AddLootGenerator(loot_generator);
    auto maps = json_data.as_object().at(JsonAttribute::MapArrayAttributes::NAME).as_array();
    for (auto& map_json : maps)
    {
        auto map = serialize_map(map_json,game.GetDefaultDogSpeed(),game.GetDefaultBagCapacity(),ioc);
        game.AddMap(map);
    }
    return game;
}

}  // namespace json_loader
