#pragma once

#include "Types.h"
#include <memory>
#include <string>

namespace ScoreWarrior::Test {
class Unit;
}

namespace ScoreWarrior::Test::Utils {

Command command_from_sring(const std::string& str);

std::string ids_log(const std::vector<std::shared_ptr<Unit>>& units);
std::string coord_log(const Coords& coords);
std::string march_finnished_log(const Coords &coords, UnitID unit_id);
std::string battle_log(const std::vector<std::shared_ptr<Unit>>& units_vec);
std::string winner_log(UnitID winner_id);

} // namespace ScoreWarrior::Test::Utils
