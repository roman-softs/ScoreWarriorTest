#include "Utils.h"

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

} // namespace ScoreWarrior::Test
