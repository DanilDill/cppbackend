#pragma once

#include <string>
#include "model/model.h"
#include <boost/json.hpp>
namespace json_responce
{
    std::string to_json(const std::vector<std::shared_ptr<model::Map>>& maps);
    std::string to_json(const model::Map& map);
    boost::json::array to_json(const model::Map::LootTypes& loots);
    boost::json::object to_json_obj(const model::Dog& dog);
    boost::json::object to_json_obj(const model::Game::LostObjects& lost_obj);

    template <typename Key,typename Value,typename Hasher>
    boost::json::object to_json_obj(const std::unordered_map<Key,Value,Hasher>& players, bool add_dog = false)
    {
        boost::json::object response_json;
        if (add_dog == false)
        {
            for (auto& player: players)
            {
                response_json[std::to_string(player.second->GetId())] = boost::json::value{{"name", player.second->GetName()}};

            }
            return response_json;
        }
        else
        {
            for (auto& player: players)
            {
                response_json[std::to_string(player.second->GetId())] = to_json_obj(*(player.second->GetDog()));
            }
            return  response_json;
        }

    }
    template <typename Key,typename Value,typename Hasher>
    std::string to_json(const std::unordered_map<Key,Value,Hasher>& players, bool add_dog = false)
    {
        if (add_dog == false)
        {

            return boost::json::serialize(to_json_obj(players,add_dog));
        }
        else
        {
            return  boost::json::serialize(boost::json::value{{"players",to_json_obj(players,add_dog)}});
        }

    }
    std::string AuthTokenJson(const std::string& token_str, int id);
    std::string ErrorJson(const std::string& code, const std::string& message);
}