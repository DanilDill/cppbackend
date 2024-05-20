#include "model.h"

#include <stdexcept>

namespace model {
using namespace std::literals;

void Game::AddMap(Map map) {
    const size_t index = maps_.size();
    if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            maps_.emplace_back(std::make_shared<Map>(std::move(map)));
        } catch (...) {
            map_id_to_index_.erase(it);
            throw;
        }
    }
}

int Game::AddPlayer(Token t, const std::string& player_name, const Map::Id& id)
    {
        auto size = players.size();
        players[t] = std::make_shared<Player>(size,player_name,FindMap(id));
        if (randomized_coord)
        {
            players[t]->SetRandomized();
        }
        return players[t]->GetId();
    }

const Game::Maps& Game::GetMaps() const noexcept
    {
        return maps_;
    }
    std::optional<std::shared_ptr<Player>> Game::FindPlayer(Token t) const
    {
        auto player = players.find(t);
        if (player != players.end())
        {
            return player->second;
        }
        return std::nullopt;
    }

const Game::Players& Game::GetPLayers()
{
    return players;
}

    const std::shared_ptr<Map> Game::FindMap(const Map::Id& id) const noexcept
    {
        if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
            return maps_.at(it->second);
        }
        return nullptr;
    }

    void Game::SetDefaultDogSpeed(double speed)
    {
        default_dogspeed = speed;
    }
    double Game::GetDefaultDogSpeed()const
    {
        return default_dogspeed;
    }



void Game::Tick(std::chrono::milliseconds  ms)
{
    for (auto player: players)
    {
        player.second->move(ms);
    }
    _loot_count = _lootGenerator->Generate(ms,_loot_count,players.size());
}

void Game::SetRandomizedCoord()
{
        randomized_coord = true;
}

bool Game::hasTicker()
{
    return ticker == nullptr;
}

void Game::SetTicker(std::chrono::milliseconds ms )
{
    auto api_strand = net::make_strand(ioc);
    ticker = std::make_shared<Ticker>(api_strand,ms,
                                      [this](std::chrono::milliseconds delta) { Tick(delta);});
    ticker->Start();
}

void Game::SetDirection(Token t, Direction direction)
{
    FindPlayer(t).value()->SetSpeed(direction);
}

void Game::AddLootGenerator(const std::shared_ptr<loot_gen::LootGenerator>loot_gen)
{
    _lootGenerator = loot_gen;
}
}  // namespace model
