/*
 * Created by vitriol1744 on 13.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#include "Builtins.hpp"

#include "Logger.hpp"

#include <cstdlib>
#include <string_view>

#include <unistd.h>

namespace Builtins
{
    std::unordered_map<std::string_view, BuiltinFunction> builtins;

    int exit(std::vector<char*>&)
    {
        return EXIT_SUCCESS;
    }
    int cd(std::vector<char*>& args)
    {
        if (args.size() < 2 || args[1] == nullptr)
            LogError("awsh: expected argument to \"cd\"");
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
