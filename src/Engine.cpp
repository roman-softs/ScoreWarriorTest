#include "Engine.h"

#include "ArrivedAction.h"
#include "CommandsFeeder.h"
#include "Unit.h"
#include "Utils.h"

#include <iostream>

namespace ScoreWarrior::Test {

using namespace ScoreWarrior::Test::Utils;

Engine::Engine(std::unique_ptr<CommandsFeeder> &&feeder)
{
    set_commands_feeder(std::move(feeder));
}

void Engine::set_commands_feeder(std::unique_ptr<CommandsFeeder> &&feeder)
{
    if (commands_feeder_) {
        commands_feeder_.reset();
    }
    commands_feeder_ = std::move(feeder);
}

void Engine::run()
{
    if (!commands_feeder_) {
        std::cout << "cannot run game, feeder is not set";
        return;
    }
    while (const auto command = commands_feeder_->get_command()) {
        logic_machine(*command);
    }
}

void Engine::logic_machine(const CommandData &data)
{
    const auto& command = data.first;
    const auto& command_options = data.second;

    const auto check_number_of_options = [&command_options] (size_t num) {
        if (command_options.size() != num)
            throw std::runtime_error("wrong number of commands");
    };

    switch (command) {
    case Command::CREATE_MAP: {
        print_message("MAP CREATED " + std::to_string(command_options[0]) + " " + std::to_string(command_options[1]));
        check_number_of_options(2);
        map_ = Pos{command_options[0], command_options[1]};
        break;
    }
    case Command::SPAWN_WARRIOR: {
        check_number_of_options(4);
        const auto uuid = command_options[0];
        const auto pos = Pos{command_options[1], command_options[2]};
        std::unique_ptr<ArrivingAction> attack = std::make_unique<MelleAttack>(command_options[3],
                [this](uint64_t unit_id) { melle_attack_cb(unit_id);});

        std::shared_ptr<Unit> warrior = std::make_shared<Unit>(uuid,
                pos, std::move(attack));
        units_on_map_[uuid] = warrior;
        print_message("WARRIOR SPAWNED " + std::to_string(uuid) + " ON " + coord_log(pos));

        break;
    }
    case Command::SPAWN_ARCHER: {
        check_number_of_options(4);
        const auto uuid = command_options[0];
        const auto pos = Pos{command_options[1], command_options[2]};
        std::unique_ptr<ArrivingAction> range = std::make_unique<RangedAttack>(command_options[3],
                [this](uint64_t unit_id) { ranged_attack_cb(unit_id);});

        std::shared_ptr<Unit> archer = std::make_shared<Unit>(uuid,
                pos, std::move(range));
        units_on_map_[uuid] = archer;
        print_message("ARCHER SPAWNED " + std::to_string(uuid) + " ON " + coord_log(pos));
        break;
    }
    case Command::MARCH: {
        check_number_of_options(3);
        const auto uuid = command_options[0];
        const auto it = units_on_map_.find(uuid);
        if (it == units_on_map_.end()) {
            std::cout << "error, no unit with such id: " << uuid << std::endl;
            break;
        }
        const auto pos = Pos{command_options[1], command_options[2]};
        print_message("MARCH STARTED " + std::to_string(uuid) + " ON " + coord_log(pos));
        it->second->move_to(pos);
        break;
    }
    case Command::WAIT: {
        check_number_of_options(1);
        const auto num_ticks = command_options[0];
        print_message("WAIT " + std::to_string(num_ticks));
        for(uint64_t i=0; i < num_ticks; ++i) {
            ticks_counter++;

            for (const auto& [id, unit] : units_on_map_) {
                unit->on_tick();
            }
            remove_killed_units();
        }
        break;
    }
}
}

void Engine::ranged_attack_cb(UnitID unit_id)
{
    const auto& unit = units_on_map_.at(unit_id);

    // TODO make safe code
    const auto& pos = unit->pos();

    auto units_in_pos = found_unit_in_pos(pos, unit_id);
    std::string log = march_finnished_log(pos, unit_id);
    if (!units_in_pos.empty()) {
        unit->kill();
        units_in_pos.push_back(unit);
        // made checking onre one unit, but it is wrong, shuold be like with warriors
        print_message(log + battle_log(units_in_pos) + winner_log(units_in_pos[0]->uuid()));
        return;
    }

    const auto& rad = unit->arriving_action()->ranged_attack();
    auto ranged_attacked_units = attack_units_in_donut(*rad, pos);

    if (!ranged_attacked_units.empty()) {
        ranged_attacked_units.push_back(unit);
        log += battle_log(ranged_attacked_units) + winner_log(unit_id);
    }
    print_message(log);
}

void Engine::melle_attack_cb(UnitID unit_id)
{
    const auto& unit = units_on_map_.at(unit_id);

    // TODO make safe code
    const auto& pos = unit->pos();

    auto units_in_pos = found_unit_in_pos(pos, unit_id);
    if (units_in_pos.empty()) {
        print_message(march_finnished_log(pos, unit_id));
        return;
    }
    const auto& power = unit->arriving_action()->melle_attack();

    bool all_dead = false; // parameter when two or more strongest units have the same power
    auto winners_id = std::pair<uint32_t, UnitID>(*power, unit_id);
    for(const auto& unit_in_pos : units_in_pos) {
        if (const auto& power = unit_in_pos->arriving_action()->melle_attack()) {
            if (*power > winners_id.first) {
                all_dead = false;
                winners_id = {*power, unit_in_pos->uuid()};
            }
            else if (*power == winners_id.first){
                all_dead = true;
            }
        }
    }

    units_in_pos.push_back(units_on_map_.at(unit_id));
    if (all_dead) {
        // TODO list of killed warriors
        print_message(march_finnished_log(pos, unit_id) + battle_log(units_in_pos) + " ALL DEAD");
    }
    else {
        std::remove_if(units_in_pos.begin(), units_in_pos.end(), [&winners_id](const auto unit){
            return unit->uuid() == winners_id.first;
        });
        print_message(march_finnished_log(pos, unit_id) + battle_log(units_in_pos) + winner_log(winners_id.second));
    }

    std::for_each(units_in_pos.begin(), units_in_pos.end(), [](const auto& unit) {
        unit->kill();
    });
}

std::vector<std::shared_ptr<Unit>> Engine::attack_units_in_donut(Coord rad, const Pos &pos)
{
    std::vector<std::shared_ptr<Unit>> units_in_range;
    for (const auto& [id, unit] : units_on_map_) {
        if (unit->state() == Unit::State::Marching) {
            continue;
        }

        const auto& curr_pos = unit->pos();
        // miss unit in the same point
        if (curr_pos.first == pos.first && curr_pos.second == pos.second) {
            continue;
        }

        // TODO add overflow protection
        const auto x_min = pos.first > rad ? pos.first - rad : 1;
        const auto x_max = pos.first + rad;
        const auto y_min = pos.second > rad ? pos.second - rad : 1;
        const auto y_max = pos.second + rad;

        if (x_min <= curr_pos.first <= x_max && y_min <= curr_pos.second <= y_max) {
            if (unit->is_killed())
                continue;

            unit->kill();
            units_in_range.push_back(unit);
        }
    }
    return units_in_range;
}

std::vector<std::shared_ptr<Unit>> Engine::found_unit_in_pos(const Pos& pos, UnitID except)
{
    std::vector<std::shared_ptr<Unit>> units_in_range;
    for (const auto& [id, unit] : units_on_map_) {
        if (unit->state() == Unit::State::Marching || id == except || unit->is_killed()) {
            continue;
        }
        if (unit->pos() == pos) {
            units_in_range.push_back(unit);
        }
    }
    return units_in_range;
}

void Engine::remove_killed_units()
{
    std::erase_if(units_on_map_, [](const auto& item) {
        auto const& [id, unit] = item;
        return unit->is_killed();
    });
}

void Engine::print_message(const std::string &message) const
{
    std::cout << "[" + std::to_string(ticks_counter) << "] " << message << std::endl;
}

} // namespace ScoreWarrior::Test
