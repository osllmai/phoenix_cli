//
// Created by Amir Kouhkan on 8/11/24.
//

#include "database_manager.h"

#include <sqlite3.h>
#include <string>
#include <iostream>


int DatabaseManager::execute_query(sqlite3 *db, const char *sql) {
    char *err = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL ERROR: " << err << std::endl;
        sqlite3_free(err);
    }
    return rc;
}

void DatabaseManager::create_tables(sqlite3 *db) {
    const char *sql_models = "CREATE TABLE IF NOT EXISTS tbl_models (id INTEGER PRIMARY KEY AUTOINCREMENT, model_name TEXT, path TEXT);";
    const char *sql_chats = "CREATE TABLE IF NOT EXISTS tbl_chat_histories (id INTEGER PRIMARY KEY AUTOINCREMENT, file_name TEXT, path TEXT);";

    DatabaseManager::execute_query(db, sql_models);
    DatabaseManager::execute_query(db, sql_chats);
}

void DatabaseManager::insert_models(sqlite3 *db, const std::string &model_name, const std::string &path) {
    const std::string query =
            "INSERT INTO tbl_models (model_name, path) VALUES ('" + model_name + "', '" + path + "');";
    DatabaseManager::execute_query(db, query.c_str());
}

void DatabaseManager::insert_chat_history(sqlite3 *db, const std::string &chat_file, const std::string &path) {
    const std::string query =
            "INSERT INTO tbl_chat_histories (file_name, path) VALUES ('" + chat_file + "', '" + path + "');";
    DatabaseManager::execute_query(db, query.c_str());
}

void DatabaseManager::read_models(sqlite3 *db) {
    const char *query = "SELECT * FROM tbl_models;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *model_name = sqlite3_column_text(stmt, 1);
            const unsigned char *path = sqlite3_column_text(stmt, 2);
            std::cout << "ID: " << id << " Model: " << model_name << " Path: " << path << std::endl;
        }
        sqlite3_finalize(stmt);
    }
}

void DatabaseManager::read_chat_histories(sqlite3 *db) {
    const char *query = "SELECT * FROM tbl_chat_histories;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *file_name = sqlite3_column_text(stmt, 1);
            const unsigned char *path = sqlite3_column_text(stmt, 2);
            std::cout << "ID: " << id << " Filename: " << file_name << " Path: " << path << std::endl;
        }
        sqlite3_finalize(stmt);
    }
}

void DatabaseManager::delete_model(sqlite3 *db, const std::string &model_name) {
    const std::string query = "DELETE FROM tbl_models WHERE model_name = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, model_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
//    DatabaseManager::execute_query(db, query.c_str());
}

std::string DatabaseManager::get_path_by_filename(sqlite3* db, const std::string& filename) {
    const std::string query = "SELECT path FROM tbl_chat_histories WHERE file_name = ?;";
    sqlite3_stmt* stmt;
    std::string path;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, filename.c_str(), -1, SQLITE_STATIC);

        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            const unsigned char* path_cstr = sqlite3_column_text(stmt, 0);
            path = reinterpret_cast<const char*>(path_cstr);
        } else if (rc != SQLITE_DONE) {
            std::cerr << "SQL ERROR: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }

    std::cout << path ;
    return path;
}

std::string DatabaseManager::get_path_by_model_name(sqlite3 *db, const std::string &model_name) {
    const std::string query = "SELECT path FROM tbl_models WHERE model_name = ?;";
    sqlite3_stmt *stmt;
    std::string path;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, model_name.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char *path_cstr = sqlite3_column_text(stmt, 0);
            path = reinterpret_cast<const char *>(path_cstr);
        }

        sqlite3_finalize(stmt);
    }

    return path;
}