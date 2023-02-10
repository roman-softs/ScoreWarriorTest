#include "Unit.h"

#include "ArrivedAction.h"
#include <cmath>

namespace ScoreWarrior::Test {

Unit::Unit(uint64_t uuid, const Coords& coords, std::unique_ptr<ArrivingAction> &&arriving_action)
    : uuid_(uuid)
    , coords_(coords)
    , arriving_action_(std::move(arriving_action))
{}

void Unit::on_tick()
{
    if (!ticks_)
        return;
    const auto test = *ticks_;
    ticks_ = test - 1;

    if (ticks_ && state() == State::Arriving) {
        set_state(State::Standing);
        ticks_.reset();
    }

    if (*ticks_ == 0) {
        set_state(State::Arriving);
        arriving_action_->exec(uuid());
        ticks_.reset();
    }
    else {
        set_state(State::Marching);
    }

    /*const auto ticks = *ticks_;

    switch (ticks) {
    case 0:
        setState(State::Standing);
        ticks_.reset();
        break;
    case 1:
        setState(State::Arriving);
    }


    if (*ticks_ == 0) {
        setState(State::Standing);
        ticks_.reset();
    }
    else if (*ticks_ == 1) {
        setState(State::Arriving);
    }
    else {
        setState(State::Arriving);
    }*/
}

void Unit::move_to(const Coords &coords)
{
    // TODO mb move to utils
    coords_ = coords;
    const auto calc_unsigned_delta = [](Coord a, Coord b)->Coord {
        if (a > b)
            return a - b;
        else
            return b - a;
    };
    const Coord dx = calc_unsigned_delta(coords_.first, coords.first);
    const Coord dy = calc_unsigned_delta(coords_.second, coords.second);

    const auto pow_x = std::pow(dx, 2);
    const auto pow_y = std::pow(dy, 2);
    //const auto sqrt = std::sqrt(pow_x - pow_y);
    const auto sqrt = std::sqrt(pow_x + pow_y);
    const auto test = static_cast<Coord>(std::ceil(sqrt));
    ticks_ = test;
}

Unit::State Unit::state() const
{
    return state_;
}

const Coords &Unit::coords() const
{
    return coords_;
}

uint64_t Unit::uuid() const
{
    return uuid_;
}

const std::unique_ptr<ArrivingAction> &Unit::arriving_action() const
{
    return arriving_action_;
}

void Unit::set_state(Unit::State state)
{
    state_ = state;
}

} // namespace ScoreWarrior::Test
