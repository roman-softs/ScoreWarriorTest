#include "Utils.h"

#include "Unit.h"
#include <stdexcept>

namespace ScoreWarrior::Test::Utils {

Command command_from_sring(const std::string &str)
{
    if (str == "CREATE_MAP")
        return Command::CREATE_MAP;
    if (str == "SPAWN_WARRIOR")
        return Command::SPAWN_WARRIOR;
    if (str == "SPAWN_ARCHER")
        return Command::SPAWN_ARCHER;
    if (str == "MARCH")
        return Command::MARCH;
    if (str == "WAIT")
        return Command::WAIT;
    throw std::runtime_error("no such command");
}

std::string ids_log(const std::vector<std::shared_ptr<Unit>>& units)
{
    std::string res;
    for (const auto& unit : units) {
        res += std::to_string(unit->uuid()) + " ";
    }
    return res.substr(0, res.size() - 1);
}

std::string coord_log(const Coords& coords)
{
    return std::to_string(coords.first) + " " + std::to_string(coords.second);
}

std::string march_finnished_log(const Coords &coords, UnitID unit_id)
{
    return "MARCH " + std::to_string(unit_id) + " FINISHED " + coord_log(coords);
};

std::string battle_log(const std::vector<std::shared_ptr<Unit>>& units_vec)
{
        return " BATTLE " + /*std::to_string(unit_id) + " " +*/ ids_log(units_vec);
}

std::string winner_log(UnitID winner_id)
{
    return " WINNER IS " + std::to_string(winner_id);
}

} // namespace ScoreWarrior::Test
