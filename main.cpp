#include "Engine.h"
#include "Parser.h"
#include "CommandsFeeder.h"

#include <iostream>
#include <filesystem>
#include <fstream>

using namespace std;

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cout << "no file path in command, application will be closed" << std::endl;
        return EXIT_FAILURE;
    }
    const std::string item_name = std::filesystem::current_path().string() + "\\" + argv[1];
    using namespace ScoreWarrior::Test;
    std::unique_ptr<CommandsFeeder> feeder;
    try {
        feeder = std::make_unique<Parser>(item_name);
    }  catch (const std::runtime_error& error) {
        std::cout << error.what() + std::string(" application will be closed") << std::endl;
        return EXIT_FAILURE;
    }

    Engine engine(std::move(feeder));
    engine.run();

    return EXIT_SUCCESS;
}
