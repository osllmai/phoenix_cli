#include "database_manager.h"
#include "directory_manager.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>

int DatabaseManager::execute_query(sqlite3 *db, const char *sql) {
    char *err = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL ERROR: " << err << std::endl;
        sqlite3_free(err);
    }
    return rc;
}

void DatabaseManager::create_tables(sqlite3 *db) {
    const char *sql_models = "CREATE TABLE IF NOT EXISTS tbl_models (id INTEGER PRIMARY KEY AUTOINCREMENT, model_name TEXT, path TEXT);";
    const char *sql_chats = "CREATE TABLE IF NOT EXISTS tbl_chat_histories (id INTEGER PRIMARY KEY AUTOINCREMENT, file_name TEXT, path TEXT);";

    if (DatabaseManager::execute_query(db, sql_models) != SQLITE_OK) {
        throw std::runtime_error("Failed to create tbl_models table.");
    }
    if (DatabaseManager::execute_query(db, sql_chats) != SQLITE_OK) {
        throw std::runtime_error("Failed to create tbl_chat_histories table.");
    }
}

void DatabaseManager::insert_models(sqlite3 *db, const std::string &model_name, const std::string &path) {
    const std::string query = "INSERT INTO tbl_models (model_name, path) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare insert statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, model_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, path.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert model: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
}

void DatabaseManager::insert_chat_history(sqlite3 *db, const std::string &chat_file, const std::string &path) {
    const std::string query = "INSERT INTO tbl_chat_histories (file_name, path) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare insert statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, chat_file.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, path.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert chat history: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
}

void DatabaseManager::read_models(sqlite3 *db) {
    const char *query = "SELECT * FROM tbl_models;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *model_name = sqlite3_column_text(stmt, 1);
            const unsigned char *path = sqlite3_column_text(stmt, 2);
            std::cout << "ID: " << id << " Model: " << model_name << " Path: " << path << "\n";
        }
    } else {
        std::cerr << "Failed to read models: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
}

void DatabaseManager::read_chat_histories(sqlite3 *db) {
    const char *query = "SELECT * FROM tbl_chat_histories;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *file_name = sqlite3_column_text(stmt, 1);
            const unsigned char *path = sqlite3_column_text(stmt, 2);
            std::cout << "ID: " << id << " Filename: " << file_name << " Path: " << path << "\n";
        }
    } else {
        std::cerr << "Failed to read chat histories: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
}

void DatabaseManager::delete_model(sqlite3 *db, const std::string &model_name) {
    const std::string query = "DELETE FROM tbl_models WHERE model_name = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, model_name.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to delete model: " << sqlite3_errmsg(db) << std::endl;
        }
    } else {
        std::cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
}

std::string DatabaseManager::get_path_by_filename(sqlite3 *db, const std::string &filename) {
    const std::string query = "SELECT path FROM tbl_chat_histories WHERE file_name = ?;";
    sqlite3_stmt *stmt;
    std::string path;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, filename.c_str(), -1, SQLITE_STATIC);

        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            const unsigned char *path_cstr = sqlite3_column_text(stmt, 0);
            path = reinterpret_cast<const char *>(path_cstr);
        } else if (rc != SQLITE_DONE) {
            std::cerr << "SQL ERROR: " << sqlite3_errmsg(db) << std::endl;
        }
    } else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    return path;
}

std::unique_ptr<sqlite3, decltype(&sqlite3_close)> DatabaseManager::open_database() {
    sqlite3 *db;
    const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        throw std::runtime_error("Failed to open database");
    }
    return {db, sqlite3_close};
}

std::string DatabaseManager::get_path_by_model_name(sqlite3 *db, const std::string &model_name) {
    const std::string query = "SELECT path FROM tbl_models WHERE model_name = ?;";
    sqlite3_stmt *stmt;
    std::string path;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, model_name.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char *path_cstr = sqlite3_column_text(stmt, 0);
            path = reinterpret_cast<const char *>(path_cstr);
        } else {
            std::cerr << "Model not found: " << model_name << std::endl;
        }
    } else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    return path;
}
