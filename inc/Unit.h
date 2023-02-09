#pragma once

#include <memory>
#include <utility>
#include <optional>
#include "Types.h"

namespace ScoreWarrior::Test {

class ArrivingAction;

class Unit
{
public:
    enum class State {
        Standing,
        Marching,
        Arriving // аналог атаки в тестовом задании, класс проектировал исходя из того что юниты могут не уметь воевать вовсе
    };
    Unit(uint64_t uuid, const Coords& coords, std::unique_ptr<ArrivingAction>&& arriving_action);

    void on_tick();
    void move_to(const Coords& coords);

    State state() const;
    Coords coords() const;
    uint64_t uuid() const;

private:
    void set_state(State state);
    State state_ = State::Standing;
    uint64_t uuid_;
    Coords coords_;
    std::optional<Coord> ticks_;
    // TODO rename to event
    std::unique_ptr<ArrivingAction> arriving_action_;
};

} // namespace ScoreWarrior::Test
