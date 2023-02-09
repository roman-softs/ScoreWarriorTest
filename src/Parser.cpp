#include "Parser.h"

#include "Utils.h"
#include <iostream>
#include <filesystem>

namespace ScoreWarrior::Test {

Parser::Parser(const std::string &file_path)
{
    stream_.open(file_path);
}

std::optional<CommandData> Parser::get_command()
{
    std::string line;
    std::getline(stream_, line);
    const std::size_t comment_pos = line.find("//");      // position of "live" in str
    std::string before_comment = line.substr(0, comment_pos);

    if (before_comment.empty())
        return {};
    return split(before_comment);
}

std::optional<CommandData> Parser::split(const std::string &str) const
{
    std::optional<Command> val_command;
    std::vector<Coord> val_params;

    std::stringstream split_stream(str);
    std::string splitted_string;
    while (split_stream >> splitted_string) { // Extract word from the stream.
        //cout << word << endl;
        if (!val_command) {
            // TODO mb move catching on level higher
            try {
                val_command = Utils::command_from_sring(splitted_string);
                continue;
            } catch (const std::runtime_error& error) {
                std::cout << error.what() << std::endl;
                return {};
            } catch (...) {
                std::cout << "unknown error while parsing" << std::endl;
                return {};
            }
        }

        try {
            val_params.emplace_back(std::stoull(splitted_string));
        }  catch (...) {
            std::cout << "wrong format" << std::endl;
        }
    }
    return CommandData{*val_command, val_params};
}

} //namespace ScoreWarrior::Test
