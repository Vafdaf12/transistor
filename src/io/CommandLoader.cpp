#include "CommandLoader.h"
#include <fstream>
#include <iostream>
#include <string>

void CommandLoader::registerCommand(const std::string& cmd, Command&& cb) {
    _commands[cmd] = cb;
}

bool CommandLoader::loadFile(const std::string& path) {
    std::ifstream file(path);
    if(!file.is_open()) {
        std::cout << "[ERROR] Failed to open file: \"" << path << "\"" << std::endl;
        return false;
    }
    std::string line;
    do {
        std::getline(file, line);
        if (line.empty()) {
            continue;
        }
        if (line[0] == ':') {
            size_t i = line.find(' ');
            std::string cmd = line.substr(1,  i-1);
            auto it = _commands.find(cmd);
            if(it == _commands.end()) {
                std::cout << "[ERROR] Unsupported command: \"" << cmd << "\"" << std::endl;
                return false;
            }
            it->second(line.substr(i+1));
        }
    }while(!file.eof());
    return true;
}
