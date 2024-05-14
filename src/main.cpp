/*
 * Created by vitriol1744 on 13.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#include <filesystem>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>

#include "Builtins.hpp"
#include "Logger.hpp"

int execute(std::vector<char*>& args)
{
    if (args[0] == nullptr) return EXIT_FAILURE;
    Builtins::BuiltinFunction builtin = Builtins::FindBultin(args[0]);
    if (builtin)
        return (builtin)(args);

    pid_t pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args.data()) == -1)
            LogError("awsh: Command not found");
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
        LogError("awsh");
    else
    {
        int status;
        do
        {
            pid_t wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

void splitArguments(std::string_view line, std::vector<char*>& arr)
{
    constexpr const char* TOKEN_DELIMETER = " \t\r\n\a";
    char* token = strtok((char*)line.data(), TOKEN_DELIMETER);
    int i = 0;

    while (token)
    {
        arr.push_back(token);
        i++;

        token = strtok(nullptr, TOKEN_DELIMETER);
    }
    arr.push_back(nullptr);
}
bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void printPrompt()
{
    passwd* pw = getpwuid (geteuid());
    std::string cwd = std::filesystem::current_path();
    cwd.erase(std::remove(cwd.begin(), cwd.end(), '\"'), cwd.end());
    std::string homedir = "/home/";
    homedir += pw->pw_name;
    replace(cwd, homedir, "~");
    std::cout << cwd << "> ";;
}

int main(int argc, char** argv)
{
    Builtins::Initialize();

    bool status = true;
    std::string line;

    do
    {
        printPrompt();

        std::getline(std::cin, line);
        std:: cout << line << "\n";
        std::vector<char*> args;
        splitArguments(line, args);
        status = execute(args);
    } while (status);
}
