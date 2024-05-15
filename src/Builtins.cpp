/*
 * Created by vitriol1744 on 13.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#include "Builtins.hpp"

#include "Logger.hpp"

#include <string_view>

#include <unistd.h>
#include <pwd.h>

namespace Builtins
{
    std::unordered_map<std::string_view, BuiltinFunction> builtins;

    int exit(std::vector<char*>& args)
    {
        if (args.size() > 3) 
        {
            LogError("exit: Too many arguments provided");
            for (auto arg : args) LogError("{}", arg);
            return true;
        }
        int status = args.size() > 1 ? atoi(args[1]) : 0;
        LogInfo("status: {}", status);
        ::exit(status);
        return status;
    }
    int cd(std::vector<char*>& args)
    {
        if (args.size() < 2 || args[1] == nullptr || strcmp(args[1], "~") == 0)
        {
            passwd* pw = getpwuid (geteuid());
            std::string homedir;
            homedir.reserve(strlen("/home/") + strlen(pw->pw_name));
            homedir = "/home/";
            homedir += pw->pw_name;
            if (chdir(homedir.data())) LogError("Failed to change directory");    
        }
        else
        {
            if (chdir(args[1]) != 0)
                LogError("awsh");
        }
        return 1;
    }

    void Initialize()
    {
        builtins["exit"] = exit;
        builtins["cd"] = cd;
    }

    BuiltinFunction FindBultin(const char* name)
    {
        if (auto it = builtins.find(name); it != builtins.end()) return it->second;
        
        return BuiltinFunction();
    }
}
