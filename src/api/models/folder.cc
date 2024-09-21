#include "api/include/models/folder.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>
#include <string>

namespace models {
    void Folder::create_folder(const UserFolder &user_folder) {
        db << "INSERT INTO folders (user_id, workspace_id, name, description, type) VALUES (?, ?, ?, ?, ?)"
           << user_folder.user_id << user_folder.workspace_id << user_folder.name << user_folder.description
           << user_folder.type;
    }

    UserFolder Folder::get_folder_by_id(const int &folder_id) {
        UserFolder user_folder;
        try {
            db
                    << "SELECT id, user_id, workspace_id, created_at, updated_at, name, description, type FROM folders WHERE id = ?"
                    << folder_id
                    >> [&](int id, std::string user_id, int workspace_id, std::string created_at,
                           std::string updated_at, std::string name, std::string description, std::string type) {
                        user_folder = UserFolder(id, user_id, workspace_id, created_at, updated_at, name, description,
                                                 type);
                    };

        } catch (const sqlite::sqlite_exception &e) {
            std::cerr << std::string(e.what());
        }
        return user_folder;
    }

    bool Folder::delete_folder(const int &folder_id) {
        try {
            db << "DELETE FROM folders WHERE id = ?" << folder_id;
            int is_deleted = 0;
            db << "SELECT changes();" >> is_deleted;
            return is_deleted > 0;
        } catch (const sqlite::sqlite_exception &e) {
            std::cerr << "SQLite error updating workspace: " << e.what() << " (" << e.get_code() << ")" << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Error updating workspace: " << e.what() << std::endl;
            return false;
        }
    }

    bool Folder::update_folder(const int &folder_id, const UserFolder &user_folder) {
        try {
            db << "UPDATE folders SET name = ?, description = ?, type = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?"
               << user_folder.name << user_folder.description << user_folder.type << user_folder.id;
            int is_updated = 0;
            db << "SELECT changes();" >> is_updated;
            return is_updated > 0;
        } catch (const sqlite::sqlite_exception &e) {
            return false;
        }
    }

    std::vector<UserFolder> Folder::user_folders_by_user(const std::string &user_id) {
        std::vector<UserFolder> folders;
        db
                << "SELECT id, user_id, workspace_id, created_at, updated_at, name, description, type FROM folders WHERE user_id = ?;"
                << user_id
                >> [&](int id, std::string user_id, int workspace_id, std::string created_at,
                       std::string updated_at,
                       std::string name, std::string description, std::string type) {
                    folders.push_back(
                            UserFolder(id, user_id, workspace_id, created_at, updated_at, name, description, type));
                };
        return folders;
    }

    std::vector<UserFolder> Folder::user_folders_by_workspace(const int &workspace_id) {
        std::vector<UserFolder> folders;
        db
                << "SELECT id, user_id, workspace_id, created_at, updated_at, name, description, type FROM folders WHERE workspace_id = ?;"
                << workspace_id
                >> [&](int id, std::string user_id, int workspace_id, std::string created_at,
                       std::string updated_at,
                       std::string name, std::string description, std::string type) {
                    folders.push_back(
                            UserFolder(id, user_id, workspace_id, created_at, updated_at, name, description, type));
                };
        return folders;
    }

    void Folder::to_json(json &j, const UserFolder &folder) {
        j = json{
                {"id",           folder.id},
                {"user_id",      folder.user_id},
                {"workspace_id", folder.workspace_id},
                {"created_at",   folder.created_at},
                {"updated_at",   folder.updated_at},
                {"name",         folder.name},
                {"description",  folder.description},
                {"type",         folder.type}
        };
    }

}
