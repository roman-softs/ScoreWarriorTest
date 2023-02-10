#include "Engine.h"

#include "CommandsFeeder.h"
#include "Unit.h"
#include "ArrivedAction.h"
#include <iostream>

namespace ScoreWarrior::Test {

namespace  {
std::string ids_to_string(const std::vector<UnitID>& ids) {
    std::string res;
    for (const auto id : ids) {
        res += std::to_string(id) + ", ";
    }
    return res.substr(0, res.size() - 2);
}

void print_march_message(const Coords &coords, UnitID unit_id, const std::vector<UnitID>& units_vec) {
    std::cout << "MARCH " + std::to_string(unit_id) + " FINISHED "
                 + std::to_string(coords.first) + " " + std::to_string(coords.second)
                 + " BATTLE" + std::to_string(coords.first) + " " + ids_to_string(units_vec)
                 + "WINNER IS " + std::to_string(unit_id) << std::endl;
};

}

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
        check_number_of_options(2);
        map_ = Coords{command_options[0], command_options[1]};
        break;
    }
    case Command::SPAWN_WARRIOR: {
        check_number_of_options(4);
        const auto uuid = command_options[0];
        const auto coords = Coords{command_options[1], command_options[2]};
        std::unique_ptr<ArrivingAction> attack = std::make_unique<MelleAttack>(command_options[3],
                [this](uint64_t unit_id) { melle_attack_cb(unit_id);});

        std::shared_ptr<Unit> warrior = std::make_shared<Unit>(uuid,
                coords, std::move(attack));
        units_on_map_[uuid] = warrior;
        break;
    }
    case Command::SPAWN_ARCHER: {
        check_number_of_options(4);
        const auto uuid = command_options[0];
        const auto coords = Coords{command_options[1], command_options[2]};
        std::unique_ptr<ArrivingAction> range = std::make_unique<RangedAttack>(command_options[3],
                [this](uint64_t unit_id) { ranged_attack_cb(unit_id);});

        std::shared_ptr<Unit> archer = std::make_shared<Unit>(uuid,
                coords, std::move(range));
        units_on_map_[uuid] = archer;
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
        const auto coords = Coords{command_options[1], command_options[2]};
        it->second->move_to(coords);
        break;
    }
    case Command::WAIT: {
        check_number_of_options(1);
        clear_attack_units();
        const auto num_ticks = command_options[0];
        for(uint64_t i=0; i < num_ticks; ++i) {
            ticks_counter++;
            for (const auto& [id, unit] : units_on_map_) {
                unit->on_tick();
                // after all units are react on tick we should check the logic of attacks
            }
        }
        break;
    }

}
}

void Engine::ranged_attack_cb(UnitID unit_id)
{
    /*
     * вообще это неправильная логика, потому что если мы принимаем один тик как единицу времени, то
     * последовательность испускания сигналов не должна играть роли,
     * первоначально я планировал написать систему которая коллекционирует всех атакующих
     * на данном тике юнитов и логику которая сопоставляет радиусы, силы и тд,
     * но в связи с тем что это слишком трудоемко для тестовой задачи (по моему мнению) решил отказаться.
     * Теперь возможен случай когда лучник убьет другого лучника, а тот так и не успеет выстрелить
     */

    /*const auto print_march_message = [&coords, &unit_id](const std::vector<UnitID>& units_vec) {
        std::cout << "MARCH " + std::to_string(unit_id) + " FINISHED "
                     + std::to_string(coords.first) + " " + std::to_string(coords.second)
                     + " BATTLE" + std::to_string(coords.first) + " " + ids_to_string(units_vec)
                     + "WINNER IS " + std::to_string(unit_id) << std::endl;
    };*/

    const auto& unit = units_on_map_.at(unit_id);

    // TODO make safe code
    const auto& coords = unit->coords();
    const auto& rad = unit->arriving_action()->ranged_attack();

    const auto ranged_attacked_units = found_units_in_donut(*rad, coords);

    print_march_message(coords, unit_id, ranged_attacked_units);
    remove_units(ranged_attacked_units);
}

void Engine::melle_attack_cb(UnitID unit_id)
{
    const auto& unit = units_on_map_.at(unit_id);

    // TODO make safe code
    const auto& coords = unit->coords();

    auto units_in_pos = found_unit_in_pos(coords, unit_id);
    const auto& power = unit->arriving_action()->melle_attack();

    bool all_dead = false; // parameter when two or more strongest units have the same power
    auto winners_id = std::pair<uint32_t, UnitID>(*power, unit_id);
    for(const auto& unit_id : units_in_pos) {
        if (const auto& power = units_on_map_.at(unit_id)->arriving_action()->melle_attack()) {
            if (*power > winners_id.first) {
                all_dead = false;
                winners_id = {*power, unit_id};
            }
            else if (*power == winners_id.first){
                all_dead = true;
            }
        }
    }

    units_in_pos.push_back(unit_id);
    if (all_dead) {
        // TODO print all dead message
        //print_march_message(coords, winners_id.first, units_in_pos);
        //units_in_pos.push_back(unit_id);
        std::cout << "ALL DEAD" << std::endl;
        remove_units(units_in_pos);
    }
    else {
        std::remove_if(units_in_pos.front(), units_in_pos.back(), [&winners_id](const auto& id){
            return id == winners_id.first;
        }
        );
        remove_units(units_in_pos);
        print_march_message(coords, winners_id.first, units_in_pos);
    }

}

void Engine::clear_attack_units()
{
    attack_ranged_units_.clear();
    attack_melle_units_.clear();
}

void Engine::run_attack()
{
    std::vector<UnitID> units_under_range_attack;
    for (const auto& [id, rad] : attack_ranged_units_) {
        const auto& coords = units_on_map_.at(id)->coords();
        const auto vec = found_units_in_donut(rad, coords);
        //units_under_range_attack.insert(units_under_range_attack.end(),vec.begin(),vec.end());
        std::cout << "MARCH " + std::to_string(id) + " FINISHED"
                     + std::to_string(coords.first) + " " + std::to_string(coords.second)
                     + "BATTLE" + std::to_string(coords.first) + " " + ids_to_string(vec)
                     + "WINNER IS" + std::to_string(id) << std::endl;
        remove_units(vec);
    }
}

std::vector<UnitID> Engine::found_units_in_donut(Coord rad, Coords pos)
{
    std::vector<UnitID> units_in_range;
    for (const auto& [id, unit] : units_on_map_) {
        if (unit->state() == Unit::State::Marching) {
            continue;
        }

        const auto& coords = unit->coords();
        // miss unit in the same point
        if (coords.first == pos.first && coords.second == pos.second) {
            continue;
        }

        // TODO add overflow protection
        const auto x_min = pos.first > rad ? pos.first - rad : 1;
        const auto x_max = pos.first + rad;
        const auto y_min = pos.second > rad ? pos.second - rad : 1;
        const auto y_max = pos.second + rad;

        if (x_min <= coords.first <= x_max && y_min <= coords.second <= y_max) {
            units_in_range.push_back(id);
        }
    }
    return units_in_range;
}

std::vector<UnitID> Engine::found_unit_in_pos(Coords pos, UnitID except)
{
    std::vector<UnitID> units_in_range;
    for (const auto& [id, unit] : units_on_map_) {
        if (unit->state() == Unit::State::Marching || id == except) {
            continue;
        }
        if (unit->coords() == pos) {
            units_in_range.push_back(id);
        }
    }
    return units_in_range;
}

void Engine::remove_units(const std::vector<UnitID> &units)
{
    for (const auto id : units) {
        units_on_map_.erase(id);
        attack_ranged_units_.erase(id);
        attack_melle_units_.erase(id);
    }
}

} // namespace ScoreWarrior::Test
