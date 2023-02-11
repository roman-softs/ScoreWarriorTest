#include "Engine.h"
#include "Parser.h"
#include "CommandsFeeder.h"

#include <iostream>
#include <filesystem>
#include <fstream>

using namespace std;

int main()
{

    std::string item_name = std::filesystem::current_path().string() + "/exmple3.txt";
    using namespace ScoreWarrior::Test;
    std::unique_ptr<CommandsFeeder> feeder = std::make_unique<Parser>(item_name);
    Engine engine(std::move(feeder));
    engine.run();

    return 0;
}
