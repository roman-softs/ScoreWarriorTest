#pragma once

#include "Types.h"
#include <optional>

namespace ScoreWarrior::Test {

class CommandsFeeder
{
public:
    virtual ~CommandsFeeder(){};
    virtual std::optional<CommandData> get_command() = 0;
};

} //namespace ScoreWarrior::Test
