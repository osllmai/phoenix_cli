#pragma once

#include <string>
#include <sqlite3.h>

namespace DatabaseManager {

    int execute_query(sqlite3 *db, const char *sql);
    void create_tables(sqlite3 *db);
    void insert_models(sqlite3 *db, const std::string &model_name, const std::string &path);
    void insert_chat_history(sqlite3 *db, const std::string &chat_file, const std::string &path);
    void read_models(sqlite3 *db);
    void read_chat_histories(sqlite3 *db);
    void delete_model(sqlite3 *db, const std::string &model_name);
    std::string get_path_by_filename(sqlite3 *db, const std::string &filename);
    std::string get_path_by_model_name(sqlite3 *db, const std::string &model_name);

    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> open_database();

};
