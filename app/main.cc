#include <iostream>

#include "commands_list.h"
#include "database_manager.h"

int main(int argc, char **argv) {
    // Database management
    sqlite3 *db;
    if(sqlite3_open("./build/bin/phoenix.db", &db) == SQLITE_OK) {
        DatabaseManager::create_tables(db);
        sqlite3_close(db);
    }
    show_commands(argc, argv);
    return 0;
}