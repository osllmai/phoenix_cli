//
// Created by Amir Kouhkan on 8/11/24.
//

#ifndef PHOENIX_DATABASE_MANAGER_H
#define PHOENIX_DATABASE_MANAGER_H

#include <string>
#include <sqlite3.h>

class DatabaseManager {
public:
    static int execute_query(sqlite3 *db, const char *sql);
    static void create_tables(sqlite3 *db);
    static void insert_models(sqlite3 *db, const std::string &model_name, const std::string &path);
    static void insert_chat_history(sqlite3 *db, const std::string &chat_file, const std::string &path);
    static void read_models(sqlite3 *db);
    static void read_chat_histories(sqlite3 *db);
    static void delete_model(sqlite3 *db, const std::string &model_name);
    static std::string get_path_by_filename(sqlite3 *db, const std::string &filename);
    static std::string get_path_by_model_name(sqlite3 *db, const std::string &model_name);
};


#endif //PHOENIX_DATABASE_MANAGER_H
