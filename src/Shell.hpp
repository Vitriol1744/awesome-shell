/*
 * Created by vitriol1744 on 14.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#pragma once

#include <vector>
#include <string_view>

namespace Shell
{
    void Initialize();
    int RunScript(std::string_view path);
    int RunPrompt();

    void PrintPrompt();
    int ExecuteCommand(std::vector<char*>& args);
    void SplitArguments(std::string_view line, std::vector<char*>& arr);
};
