//
// Created by Amir Kouhkan on 9/1/24.
//

#pragma once

#include <sqlite_modern_cpp.h>

namespace models {
    extern sqlite::database db;

    void init_db();
}
