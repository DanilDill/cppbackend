
#include <string_view>
#include "post_handler.h"
#include <boost/json.hpp>

#include "json_response.h"
namespace http_handler
{
    post_handler::post_handler(StringRequest&& request, model::Game& game, file::file_loader& root):
    default_handler(std::move(request)),game_(game),wwwroot(root)
    {
    }


    std::pair<std::string,std::string> post_handler::parse()
    {
        auto json_data =  boost::json::parse(_req.body().data());

        auto map_id_iter =  json_data.as_object().find("mapId");
        std::string map_id = "";
        std::string user_name = "";
        if (auto user_name_iter = json_data.as_object().find("userName"); user_name_iter != json_data.as_object().end())
        {
            user_name = user_name_iter->value().as_string().data();
        }
        if ( map_id_iter =  json_data.as_object().find("mapId"); map_id_iter != json_data.as_object().end())
        {
            map_id = map_id_iter->value().as_string().data();
        }
        return std::make_pair(std::move(user_name),std::move(map_id));

    }

}