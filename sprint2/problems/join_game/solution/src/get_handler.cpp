#include "get_handler.h"
#include "json_response.h"

namespace http_handler
{
    get_handler::get_handler(StringRequest&&request, model::Game& game, file::file_loader& root):
    _req(request),wwwroot(root),game_(game)
    {
    }

    std::variant <StringResponse, FileResponse> get_handler::execute()
    {
        auto target = _req.target();
        if (target.starts_with("/api/"))
        {
            if (target == "/api/v1/maps")
            {
                return  HandleGetMaps();
            }
            if (target.starts_with("/api/v1/maps/"))
            {
                std::string map_id ( target.substr("/api/v1/maps/"sv.size()));
                return HandleGetMap(map_id);
            }
            return HandleBadRequest();
        }

        target = target == "/" ? "index.html" : target.substr(1);
        auto response_file =  wwwroot.try_get(target);
        if (response_file)
        {
            auto res  = std::move((*response_file));
            return res;
        }
        return HandleNotFound();
    }

    StringResponse get_handler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive,
                                      std::string_view content_type)
    {
        StringResponse response(status, http_version);
        response.set(http::field::content_type, content_type);
        response.body() = body;
        response.content_length(body.size());
        response.keep_alive(keep_alive);
        return response;
    }


    StringResponse get_handler::HandleGetMaps()
    {
        auto maps =  game_.GetMaps();
        auto maps_json_str = json_responce::to_json(maps);
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::ok,maps_json_str);
    }
    StringResponse get_handler::HandleGetMap(const std::string& map_id)
    {
        auto map  = game_.FindMap(model::Map::Id(map_id));
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };

        if (map)
        {
            std::string map_json = json_responce::to_json(*map);
            return text_response(http::status::ok,map_json);
        }
        return text_response(http::status::not_found,json_responce::NotFoundJson());
    }

    StringResponse get_handler::HandleNotFound()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::TEXT_PLAIN);
        };
        return text_response(http::status::not_found,"ERROR 404 Not found");
    }
    StringResponse get_handler::HandleBadRequest()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::bad_request,json_responce::BadRequestJson());
    }
}

