#include "api/include/models/file.h"
#include "api/include/models/database.h"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace models {
    void File::create_file(const UserFile &user_file) {
        db
                << "INSERT INTO files (user_id, folder_id, created_at, updated_at, sharing, description, file_path, name, size, tokens, type) "
                   "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
                << user_file.user_id << user_file.folder_id << user_file.created_at
                << user_file.updated_at << user_file.sharing
                << user_file.description << user_file.file_path << user_file.name << user_file.size << user_file.tokens
                << user_file.type;
    }

    bool File::update_file(const int &file_id, const UserFile &user_file) {
        try {
            db
                    << "UPDATE files SET folder_id = ?, updated_at = CURRENT_TIMESTAMP, sharing = ?, description = ?, file_path = ?, name = ?, size = ?, tokens = ?, type = ? "
                       "WHERE id = ?"
                    << user_file.folder_id << user_file.sharing << user_file.description << user_file.file_path
                    << user_file.name << user_file.size
                    << user_file.tokens << user_file.type << file_id;
            int is_updated = 0;
            db << "SELECT changes();" >> is_updated;
            return is_updated > 0;
        } catch (const sqlite::sqlite_exception &e) {
            return false;
        }
    }

    bool File::delete_file(const int &file_id) {
        try {
            db << "DELETE FROM files WHERE id = ?" << file_id;
            int is_deleted = 0;
            db << "SELECT changes();" >> is_deleted;
            return is_deleted > 0;
        } catch (const sqlite::sqlite_exception &e) {
            return false;
        }
    }

    UserFile File::get_file_by_id(const int &file_id) {
        UserFile file;
        db
                << "SELECT id, user_id, folder_id, created_at, updated_at, sharing, description, file_path, name, size, tokens, type "
                   "FROM files WHERE id = ?"
                << file_id
                >> [&](int id, std::string user_id, int folder_id, std::string created_at,
                       std::string updated_at, std::string sharing, std::string description, std::string file_path,
                       std::string name, int size, int tokens, std::string type) {
                    file = UserFile(id, user_id, folder_id, created_at, updated_at, sharing, description, file_path,
                                    name, size,
                                    tokens, type);
                };
        return file;
    }

    void models::File::to_json(json &j, const UserFile &user_file) {
        j = json{
                {"id",          user_file.id},
                {"user_id",     user_file.user_id},
                {"folder_id",   user_file.folder_id},
                {"created_at",  user_file.created_at},
                {"updated_at",  user_file.updated_at},
                {"sharing",     user_file.sharing},
                {"description", user_file.description},
                {"file_path",   user_file.file_path},
                {"name",        user_file.name},
                {"size",        user_file.size},
                {"tokens",      user_file.tokens},
                {"type",        user_file.type}
        };
    }

    std::vector<UserFile> File::files_by_user(const std::string &user_id) {
        std::vector<UserFile> files;
        try {
            db
                    << "SELECT id, user_id, folder_id, created_at, updated_at, sharing, description, file_path, name, size, tokens, type "
                       "FROM files WHERE user_id = ?;"
                    << user_id
                    >> [&](const int &id, const std::string &user_id, const int &folder_id,
                           const std::string &created_at, const std::string &updated_at, const std::string &sharing,
                           const std::string &description, const std::string &file_path, const std::string &name,
                           int size, int tokens, const std::string &type) {

                        files.push_back(UserFile(id, user_id, folder_id, created_at, updated_at, sharing, description,
                                                 file_path, name,
                                                 size, tokens, type));
                    };
        } catch (const std::exception &e) {
            std::cerr << "Error fetching files: " << e.what() << std::endl;
        }

        return files;
    }

    std::vector<UserFile> File::files_by_folder(const int &folder_id) {
        std::vector<UserFile> files;
        try {
            db
                    << "SELECT id, user_id, folder_id, created_at, updated_at, sharing, description, file_path, name, size, tokens, type "
                       "FROM files WHERE folder_id = ?;"
                    << folder_id
                    >> [&](const int &id, const std::string &user_id, const int &folder_id,
                           const std::string &created_at, const std::string &updated_at, const std::string &sharing,
                           const std::string &description, const std::string &file_path, const std::string &name,
                           int size, int tokens, const std::string &type) {

                        files.push_back(UserFile(id, user_id, folder_id, created_at, updated_at, sharing, description,
                                                 file_path, name,
                                                 size, tokens, type));
                    };
        } catch (const std::exception &e) {
            std::cerr << "Error fetching files: " << e.what() << std::endl;
        }

        return files;
    }

    std::vector<UserFile> File::get_files_by_workspace_id(const int &workspace_id) {
        std::vector<UserFile> files;
        try {
            db << "SELECT f.id, f.user_id, f.folder_id, f.created_at, f.updated_at, f.sharing, f.description, "
                  "f.file_path, f.name, f.size, f.tokens, f.type "
                  "FROM files f "
                  "JOIN folders fo ON f.folder_id = fo.id "
                  "WHERE fo.workspace_id = ?;"
                    << workspace_id
                    >> [&](const int &id, const std::string &user_id, const int &folder_id,
                           const std::string &created_at, const std::string &updated_at, const std::string &sharing,
                           const std::string &description, const std::string &file_path, const std::string &name,
                           int size, int tokens, const std::string &type) {

                        files.push_back(UserFile(id, user_id, folder_id, created_at, updated_at, sharing, description,
                                                 file_path, name,
                                                 size, tokens, type));
                    };
        } catch (const std::exception &e) {
            std::cerr << "Error fetching files: " << e.what() << std::endl;
        }

        return files;
    }
}