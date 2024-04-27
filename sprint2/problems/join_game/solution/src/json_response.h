#pragma once
#include <string>
#include "model.h"
#include <boost/json.hpp>
namespace json_responce
{
    std::string to_json(const std::vector<model::Map>& maps);
    std::string to_json(const model::Map& map);
    template <typename Key,typename Value,typename Hasher>
    std::string to_json(const std::unordered_map<Key,Value,Hasher>& players)
    {
        boost::json::object response_json;
        for (auto& player: players)
        {
            response_json[std::to_string(player.second.GetId())] = boost::json::value{{"name", player.second.GetName()}};
            return boost::json::serialize(response_json);
        }
    }
    std::string AuthTokenJson(const std::string& token_str, int id);

    //std::string NotFoundJson();
  //  std::string BadRequestJson();
    std::string ErrorJson(const std::string& code, const std::string& message);
}