
#include "post_handler.h"
#include <boost/json.hpp>
namespace http_handler
{
    post_handler::post_handler(StringRequest&& request, model::Game& game, file::file_loader& root):
    _req(request),game_(game),wwwroot(root)
    {
    }

    std::pair<std::string,std::string> post_handler::parse()
    {
        auto json_data =  boost::json::parse(_req.body().data());
        std::string user_name = json_data.as_object().at("userName").as_string().data();
        std::string map_id =  json_data.as_object().at("mapId").as_string().data();
        return std::make_pair(std::move(user_name),std::move(map_id));

    }

    StringResponse post_handler::execute()
    {
        std::string_view target = _req.target(); // /api/v1/game/join
        if (target == "/api/v1/game/join")
        {
            if (_req[http::field::content_type] == "application/json")
            {
                 auto gamer  = parse();
                if (gamer.first.empty())
                {
                    //bad request {"code": "invalidArgument", "message": "Invalid name"}
                }
                if (gamer.second.empty())
                {
                    // bad request {"code": "invalidArgument", "message": "Invalid map"}
                }
                auto map = game_.FindMap(model::Map::Id(gamer.second));
                if (!map)
                {
                    // {"code": "mapNotFound", "message": "Map not found"}
                }
            }
            //...
        }
        //BadRequest
    }
}