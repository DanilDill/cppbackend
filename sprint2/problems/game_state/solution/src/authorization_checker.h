#include "beast.h"
#include "model.h"
#include <optional>
#include "json_response.h"
namespace http_handler
{
    using StringRequest = http::request<http::string_body>;
    class AuthorizationChecker
    {
    private:
        const StringRequest&  _req;
        const model::Game& _game_ref;
    public:
       explicit AuthorizationChecker(const StringRequest& request, const model::Game& game):
        _req(request),_game_ref(game){};
        std::optional<std::string> check()
        {
            auto auth_str = _req[http::field::authorization];
            if (auth_str == "" or auth_str.substr("Bearer"sv.size()) == "")
            {
                return json_responce::ErrorJson("invalidToken","Authorization header is missing");
            }
            auto token = auth_str.substr("Bearer "sv.size());

            if(_game_ref.FindPlayer(Token(std::string(token))))
            {
                return std::nullopt;
            }
            return json_responce::ErrorJson("unknownToken","Player token has not been found");
        };
    };
}
