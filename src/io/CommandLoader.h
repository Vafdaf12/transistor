#pragma once

#include <string>
#include <unordered_map>
#include <functional>

class CommandLoader {
public:
    using Command = std::function<void(const std::string&)>;

    void registerCommand(const std::string& cmd, Command&& cb);

    bool loadFile(const std::string& path);
private:
    std::unordered_map<std::string, Command> _commands;
};