#pragma once

//#include <utility>
#include <cstdint>
#include <vector>

namespace ScoreWarrior::Test {

using Coord = uint64_t;
using Coords = std::pair<Coord, Coord>;

enum class Command
{
    CREATE_MAP,
    SPAWN_WARRIOR,
    SPAWN_ARCHER,
    MARCH,
    WAIT
};

using CommandData = std::pair<Command, std::vector<Coord>>;

} // namespace ScoreWarrior::Test
