#pragma once

#include <string>
#include "model.h"
#include <boost/json.hpp>
namespace json_responce
{
    std::string to_json(const std::vector<std::shared_ptr<model::Map>>& maps);
    std::string to_json(const model::Map& map);
    boost::json::object to_json_obj(const model::Dog& dog);

    template <typename Key,typename Value,typename Hasher>
    std::string to_json(const std::unordered_map<Key,Value,Hasher>& players, bool add_dog = false)
    {
        boost::json::object response_json;
        if (add_dog == false)
        {
            for (auto& player: players)
            {
                response_json[std::to_string(player.second.GetId())] = boost::json::value{{"name", player.second.GetName()}};

            }
            return boost::json::serialize(response_json);
        }
        else
        {
            for (auto& player: players)
            {
                response_json[std::to_string(player.second.GetId())] = to_json_obj(*(player.second.GetDog()));
            }
            return  boost::json::serialize(boost::json::value{{"players",response_json}});
        }

    }
    std::string AuthTokenJson(const std::string& token_str, int id);
    std::string ErrorJson(const std::string& code, const std::string& message);
}