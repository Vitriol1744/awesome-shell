/*
 * Created by vitriol1744 on 13.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#include "Shell.hpp"

int main(int argc, char** argv)
{
    Shell::Initialize();
    Shell::Run(argc, argv);

    return 0;
}
