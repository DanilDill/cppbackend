#define _USE_MATH_DEFINES
#include <catch2/catch_test_macros.hpp>

#include "../src/collision_detector.h"
// Напишите здесь тесты для функции collision_detector::FindGatherEvents
class Provider : public collision_detector::ItemGathererProvider
{
private:
    std::vector<collision_detector::Item>_items;
    std::vector<collision_detector::Gatherer> _gatherers;
public:
    Provider(const std::vector<collision_detector::Item>& items, std::vector<collision_detector::Gatherer>& gatherers):
            _items(items), _gatherers(gatherers)
    {
    };
    size_t ItemsCount() const override{return _items.size();};
    collision_detector::Item GetItem(size_t idx) const override{ return _items.at(idx);};
    size_t GatherersCount() const override{return  _gatherers.size();};
    collision_detector::Gatherer GetGatherer(size_t idx) const override{return _gatherers.at(idx);};
};

bool operator==(const std::vector<collision_detector::GatheringEvent>&left, const std::vector<collision_detector::GatheringEvent>& right)
{
    if (left.size() != right.size())
    {
        return false;
    }
    for (auto i = left.begin(), j = right.begin();j != right.end() && i != left.end(); ++i,++j)
    {
        if ( i != j)
        {
            return false;
        }
    }
    return true;
}
bool operator==(const collision_detector::GatheringEvent& left, const collision_detector::GatheringEvent& right)
{
    return  left.item_id == right.item_id && left.gatherer_id == right.gatherer_id &&
            left.sq_distance == right.sq_distance && left.time == right.time;
}

TEST_CASE()
{
    std::vector<collision_detector::Item> items { {{},0.2}, {{1,40},0.5}};
    std::vector<collision_detector::Gatherer>gatherers
    {
                collision_detector::Gatherer{geom::Point2D{0.0,0.0},geom::Point2D{0.4,0.0},0.5}
    };
    Provider provider(items,gatherers);
    auto events = collision_detector::FindGatherEvents(provider);
    CHECK(events != collision_detector::FindGatherEvents_Wrong1(provider));
    CHECK(events[0] == collision_detector::GatheringEvent{});
    CHECK(collision_detector::FindGatherEvents(provider) != collision_detector::FindGatherEvents_Wrong2(provider));
    CHECK(collision_detector::FindGatherEvents(provider) != collision_detector::FindGatherEvents_Wrong3(provider));
    CHECK(collision_detector::FindGatherEvents(provider) != collision_detector::FindGatherEvents_Wrong4(provider));

}
