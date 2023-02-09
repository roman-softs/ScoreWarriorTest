#pragma once

#include "CommandsFeeder.h"
#include <string>
#include <fstream>

namespace ScoreWarrior::Test {

class Parser : public CommandsFeeder
{
public:
    Parser(const std::string& file_path);
    Parser(Parser&&) = delete;
    Parser(const Parser& parser) = delete;

    std::optional<CommandData> get_command() override;

private:
    std::optional<CommandData> split(const std::string& str) const;
    std::ifstream stream_;
};
} //namespace ScoreWarrior::Test
