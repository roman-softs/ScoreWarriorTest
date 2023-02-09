#include "Engine.h"
#include "Parser.h"
#include "CommandsFeeder.h"

#include <iostream>
#include <filesystem>
#include <fstream>

using namespace std;

int main()
{

    std::string item_name = std::filesystem::current_path().string() + "/exmple2.txt";
    /*std::ifstream nameFileout;
    nameFileout.open(item_name);

    std::string line;
    while (std::getline(nameFileout, line)) {
        std::size_t pos = line.find("//");      // position of "live" in str
        std::string str3 = line.substr(0, pos);
        //cout << str3 << endl;

        stringstream ss(str3);
        string word;
        while (ss >> word) { // Extract word from the stream.
            cout << word << endl;
        }

    }*/

    using namespace ScoreWarrior::Test;
    std::unique_ptr<CommandsFeeder> feeder = std::make_unique<Parser>(item_name);
    Engine engine(std::move(feeder));
    engine.run();

    return 0;
}
