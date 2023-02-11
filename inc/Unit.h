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
    Unit(uint64_t uuid, const Pos& pos, std::unique_ptr<ArrivingAction>&& arriving_action);

    void on_tick();
    void move_to(const Pos& pos);

    State state() const;
    const Pos& pos() const;
    uint64_t uuid() const;

    const std::unique_ptr<ArrivingAction>& arriving_action() const;
    void kill();
    bool is_killed() const;

private:
    void set_state(State state);
    State state_ = State::Standing;
    uint64_t uuid_;
    Pos pos_;
    std::optional<Coord> ticks_;
    // TODO rename to event
    std::unique_ptr<ArrivingAction> arriving_action_;
    bool is_killed_ = false;
};

} // namespace ScoreWarrior::Test
