#include <iostream>

#include "commands_list.h"
#include "database_manager.h"
#include "directory_manager.h"

int main(int argc, char **argv) {
    // Database management
    sqlite3 *db;
    const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
        DatabaseManager::create_tables(db);
        sqlite3_close(db);
    } else {
        std::cout << "Can't open" << std::endl;
    }

    show_commands(argc, argv);
    return 0;
}
