/*
 * Created by vitriol1744 on 14.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#include "Shell.hpp"

#include "Builtins.hpp"
#include "Types.hpp"
#include "Logger.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <cstring>

#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <setjmp.h>

bool substituteString(std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void substituteHomeDir(std::string& str)
{
    passwd* pw = getpwuid (geteuid());
    std::string cwd = std::filesystem::current_path();
    cwd.erase(std::remove(cwd.begin(), cwd.end(), '\"'), cwd.end());
    std::string homedir;
    homedir.reserve(strlen("/home/") + strlen(pw->pw_name));
    homedir = "/home/";
    homedir += pw->pw_name;
    substituteString(str, "~", homedir);
}

static jmp_buf env;

static void sigintHandler(int signo)
{
    siglongjmp(env, 42);
}

struct Command
{
    std::string command;
    bool background = false;
    Command* redirect = nullptr;
    Command* onSuccess = nullptr;
    Command* onFailure = nullptr;
};

void parseCommand(Command* command)
{
    usize it = command->command.find_first_of("|&><");
    if (it == std::string::npos) return;
    Command* next = new Command;
    usize firstChar = it;
    while (!isalpha(command->command[firstChar])) firstChar++;
    next->command = std::string(&command->command[firstChar]);

    switch (command->command[it])
    {
        case '|':
            if (command->command[it+1] == '|')
                command->onFailure = next;
            else
                command->redirect = next;
            break;
        case '&':
            if (command->command[it+1] == '&')
                command->onSuccess = next;
            else command->background = true;
            break;
    }
    parseCommand(next);
    command->command.resize(it);
}

void PrintCommand(Command* command, usize tabsize = 0)
{
    if (!command)
    {
        std::cout << std::endl;
        return;
    }
    for (usize i = 0; i < tabsize; i++) std::cout << " ";
    std::cout << command->command << "\n";
    tabsize += 4;
    std::cout << "onSuccess";
    PrintCommand(command->onSuccess, tabsize);
    std::cout << "onFailure";
    PrintCommand(command->onFailure, tabsize);
    std::cout << "redirect";
    PrintCommand(command->redirect, tabsize);
}

void splitCommands(std::string& line, std::vector<Command*>& out)
{
    std::istringstream iss(line);
    std::string token;

    while (std::getline(iss, token, ';'))
    {
        token.erase(token.begin(), std::find_if(token.begin(), token.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        Command* command = new Command;
        command->command = token;
        parseCommand(command);
        out.push_back(command);
    }
    for (auto c : out) LogInfo("{}", c->command);
}

namespace Shell
{
    static int lastStatus = 0;

    int ExecuteCommand(Command* command);
    void Initialize()
    {
        Builtins::Initialize();
        signal (SIGINT, SIG_IGN);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGCHLD, SIG_IGN);
        signal(SIGINT, sigintHandler);
    }
    int Run(int argc, char** argv)
    {
        bool status = true;
        std::string line;

        do
        {
            if (feof(stdin)) exit(0);
            if (sigsetjmp(env, 1) == 42) continue;
            Shell::PrintPrompt();

            std::getline(std::cin, line);
            if (line.empty()) continue;
            std::vector<Command*> commands;
            splitCommands(line, commands);
            substituteString(commands[0]->command, "$?", std::to_string(lastStatus));
            std::vector<char*> args;
            Shell::SplitArguments(line, args);
            status = Shell::ExecuteCommand(commands[0]);
        } while (status);

        return status;
    }

    void PrintPrompt()
    {
        passwd* pw = getpwuid (geteuid());
        std::string cwd = std::filesystem::current_path();
        cwd.erase(std::remove(cwd.begin(), cwd.end(), '\"'), cwd.end());
        std::string homedir;
        homedir.reserve(strlen("/home/") + strlen(pw->pw_name));
        homedir = "/home/";
        homedir += pw->pw_name;
        substituteString(cwd, homedir, "~");
        std::cout << cwd << "> ";;
    }
    int ExecuteCommand(Command* command)
    {
        if (!command) return 1;
        std::vector<char*> args;
        Shell::SplitArguments(command->command, args);

        if (args[0] == nullptr) return EXIT_FAILURE;
        Builtins::BuiltinFunction builtin = Builtins::FindBultin(args[0]);
        if (builtin)
            return (builtin)(args);

        pid_t pid = fork();
        if (pid == 0)
        {
            execvp(args[0], args.data());
            LogError("Command not found: {}", args[0]);
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
            LogError("awsh");
        else
        {
            int status;
            pid_t wpid;
            do
            {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status)); 
            LogInfo("wpid: {}", wpid);
            if (WIFEXITED(status)) 
            {
                lastStatus = WEXITSTATUS(status);
                LogInfo("exit status: {}", lastStatus);
            }
            if (wpid == -1)
                ExecuteCommand(command->onFailure);
            else
                ExecuteCommand(command->onSuccess);
        }

        return 1;
    }
    
    void SplitArguments(std::string_view line, std::vector<char*>& arr)
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
}
