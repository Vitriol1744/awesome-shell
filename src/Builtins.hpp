/*
 * Created by vitriol1744 on 13.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#pragma once

#include <functional>

namespace Builtins
{
    using BuiltinFunction = std::function<int(std::vector<char*>&)>;

    void Initialize();
    BuiltinFunction FindBultin(const char* name);

    int exit(std::vector<char*>&);
}
