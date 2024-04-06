
#include "request_handler.h"
#include "json_response.h"
namespace http_handler
{
    StringResponse RequestHandler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
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

    StringResponse RequestHandler::HandleHead(StringRequest&& req)
    {
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        return text_response(http::status::ok, ""sv);
    }

    StringResponse RequestHandler::HandleGetMap(const std::string& map_id, StringRequest&& req)
    {

      auto map  = game_.FindMap(model::Map::Id(map_id));
      const auto text_response = [&req, this](http::status status, std::string_view text)
      {
          return MakeStringResponse(status, text, req.version(), req.keep_alive(),ContentType::APPLICATION_JSON);
      };

      if (map)
      {
          std::string map_json = json_responce::to_json(*map);
          return text_response(http::status::ok,map_json);
      }
        return text_response(http::status::not_found,json_responce::NotFoundJson());
    }


    StringResponse RequestHandler::HandleGetMaps(StringRequest&& req)
    {
       auto maps =  game_.GetMaps();
       auto maps_json_str = json_responce::to_json(maps);
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::ok,maps_json_str);
    }




    StringResponse  RequestHandler::HandleBadRequest(StringRequest&& req)
    {

        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::bad_request,json_responce::BadRequestJson());
    }

    StringResponse  RequestHandler::HandleNotFound(StringRequest&& req)
    {
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::not_found,json_responce::NotFoundJson());
    }
    StringResponse RequestHandler::HandleUnexpected(StringRequest&& req)
    {
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        return text_response(http::status::method_not_allowed,"Invalid method");
    }



}  // namespace http_handler
