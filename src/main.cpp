/*
 * Created by vitriol1744 on 13.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#include "Shell.hpp"
#include "Logger.hpp"

int main(int argc, char** argv)
{
    Shell::Initialize();
    if (argc > 2)
    {
        LogError("Too many arguments provided");
        return EXIT_FAILURE;
    }
    
    return argc == 2 ? Shell::RunScript(argv[1]) : Shell::RunPrompt();
}
