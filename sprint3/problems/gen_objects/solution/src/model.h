#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <atomic>
#include <memory>
#include <optional>
#include "tokenizer.h"

#include "ticker.h"
#include "loot_generator.h"
#include <boost/asio.hpp>
#include "point.h"
#include "direction.h"
#include "game_map.h"
#include "player.h"

namespace model
{

class Game {
public:
    using Maps = std::vector<std::shared_ptr<Map>>;
    using PlayerHasher = util::TaggedHasher<Token>;
    using Players = std::unordered_map<Token,Player,PlayerHasher>;
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
    Game(boost::asio::io_context& context): ioc(context){};

    void Tick(std::chrono::milliseconds  ms);
    void SetRandomizedCoord();
    void SetTicker(std::chrono::milliseconds ms );
    void SetDefaultDogSpeed(double speed);
    void SetDirection(Token t, Direction direction);
    double GetDefaultDogSpeed()const;
    void AddMap(Map map);
    int AddPlayer(Token t, const std::string& player_name, const Map::Id& id);
    void AddLootGenerator(const std::shared_ptr<loot_gen::LootGenerator>loot_gen);
    const Maps& GetMaps() const noexcept;
    std::optional<Player> FindPlayer(Token t) const;
    const Players& GetPLayers();
    const std::shared_ptr<Map> FindMap(const Map::Id& id) const noexcept;
    bool hasTicker();

private:
    std::shared_ptr<Ticker> ticker = nullptr;
    bool randomized_coord = false;
    boost::asio::io_context& ioc;
    Players players;
    Maps maps_;
    MapIdToIndex map_id_to_index_;
    double default_dogspeed;
    std::shared_ptr<loot_gen::LootGenerator> _lootGenerator = nullptr;
    unsigned _loot_count;
};

}  // namespace model
