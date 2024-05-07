#include "head_handler.h"

namespace http_handler
{

    head_handler::head_handler(StringRequest&&request, model::Game& game, file::file_loader& root):
            default_handler(std::move(request)), wwwroot(root), game_(game)
    {}

}