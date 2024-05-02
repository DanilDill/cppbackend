#include "beast.h"
#include "model.h"
#include <optional>
#include "json_response.h"
#include <regex>
namespace http_handler
{
    using StringRequest = http::request<http::string_body>;
    class AuthorizationChecker
    {
    private:
        std::regex _regex; //("Bearer [a-z,A-Z,0-9]{32}");
        const StringRequest&  _req;
        const model::Game& _game_ref;
        Token lastToken = Token ("");
    public:
       explicit AuthorizationChecker(const StringRequest& request, const model::Game& game):
        _req(request),_game_ref(game),_regex("Bearer [a-z,A-Z,0-9]{32}"){};
        std::optional<std::string> check()
        {

            auto auth_str = _req[http::field::authorization];
            if (std::regex_search(std::string(auth_str), _regex))
            {
                lastToken = Token(std::string(auth_str.substr("Bearer "sv.size())));
                if(_game_ref.FindPlayer(lastToken))
                {
                    return std::nullopt;
                }
                return json_responce::ErrorJson("unknownToken","Player token has not been found");
            }
            else
            {
                return json_responce::ErrorJson("invalidToken","Authorization header is required");
            }
        };
        Token getLastToken()
        {
            return lastToken;
        }
    };
}
