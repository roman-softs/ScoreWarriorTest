#pragma once

#include "Types.h"
#include <memory>
#include <map>
#include <optional>

namespace ScoreWarrior::Test {

class CommandsFeeder;
class Unit;

class Engine
{
public:
    Engine(std::unique_ptr<CommandsFeeder>&& feeder);
    Engine(Engine&&) = delete;
    Engine(const Engine& feeder) = delete;

    void set_commands_feeder(std::unique_ptr<CommandsFeeder>&& feeder);
    void run();

private:
    void logic_machine(const CommandData& data);

    void ranged_attack_cb(UnitID unit_id);
    void melle_attack_cb(UnitID unit_id);

    void remove_killed_units();

    void print_message(const std::string& message) const;

    //void run_range_unit_attack(Coord rad, const Unit& unit);
    std::vector<std::shared_ptr<Unit>> attack_units_in_donut(Coord rad, const  Pos& pos);
    std::vector<std::shared_ptr<Unit>> found_unit_in_pos(const Pos& pos, UnitID except);

    std::unique_ptr<CommandsFeeder> commands_feeder_;

    std::optional<Pos> map_;
    std::map<UnitID, std::shared_ptr<Unit>> units_on_map_;

    uint32_t ticks_counter = 0;
};

} // namespace ScoreWarrior::Test
