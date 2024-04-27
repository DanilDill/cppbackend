#include "head_handler.h"
#include "json_response.h"
namespace http_handler
{

    head_handler::head_handler(StringRequest&&request, model::Game& game, file::file_loader& root):
            _req(request), wwwroot(root), game_(game)
    {}

    StringResponse head_handler::execute()
    {
        auto target = _req.target();
        if (target.starts_with("/api/")) {
            if (target == "/api/v1/maps") {
                return HandleGetMaps();
            }
            if (target.starts_with("/api/v1/maps/")) {
                std::string map_id(target.substr("/api/v1/maps/"sv.size()));
                return HandleGetMap(map_id);
            }
            if (target == "/api/v1/game/players") {
                auto auth_str = _req[http::field::authorization];
                if (auth_str == "")
                {
                    return HandleUnathorized();
                }
                auto token = auth_str.substr("Bearer "sv.size());

                if(game_.FindPlayer(Token(std::string(token))))
                {
                    return HandlePlayersList();
                }
                return PLayerNotFound();
            }
            return HandleNotAllowed();
        }

        target = target == "/" ? "index.html" : target.substr(1);
        auto result = wwwroot.try_get_head(target);
        if (result)
        {
            return *result;
        }
        return HandleNotFound();
    }


    StringResponse head_handler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
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

    StringResponse head_handler::HandlePlayersList()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::ok,"");
    }

    StringResponse head_handler::HandleGetMaps()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::ok,"");
    }
    StringResponse head_handler::HandleUnathorized()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        auto response = text_response(http::status::unauthorized, "");
        response.set(http::field::cache_control,"no-cache");
        return response;
    }
    StringResponse head_handler::HandleGetMap(const std::string& map_id)
    {
        auto map  = game_.FindMap(model::Map::Id(map_id));
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };

        if (map)
        {
            return text_response(http::status::ok,"");
        }
        return text_response(http::status::not_found,"");
    }

    StringResponse head_handler::HandleNotFound()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::not_found,"");
    }
    StringResponse head_handler::HandleBadRequest()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::bad_request,"");
    }
    StringResponse head_handler::PLayerNotFound()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::unauthorized,"");
    }

    StringResponse head_handler::HandleNotAllowed()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        auto  resp =  text_response(http::status::method_not_allowed,json_responce::ErrorJson("invalidMethod","Invalid method"));
        resp.set(http::field::cache_control,"no-cache");
        resp.set(http::field::allow,"POST");
        return resp;
    }
}