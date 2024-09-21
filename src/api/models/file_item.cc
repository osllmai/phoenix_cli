#include "api/include/models/file_item.h"
#include "api/include/models/database.h"

#include <string>
#include <vector>
#include <sqlite_modern_cpp.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iterator>

using json = nlohmann::json;

namespace models {

// Serialize std::vector<float> to a string
    std::string serialize_vector(const std::vector<float> &vec) {
        std::ostringstream oss;
        for (const auto &val: vec) {
            oss << val << ' ';
        }
        return oss.str();
    }

// Deserialize std::string to std::vector<float>
    std::vector<float> deserialize_vector(const std::string &str) {
        std::istringstream iss(str);
        return std::vector<float>(std::istream_iterator<float>(iss), std::istream_iterator<float>());
    }

    // Function to calculate cosine similarity
    float calculate_cosine_similarity(const std::vector<float> &a, const std::vector<float> &b) {
        float dot_product = 0.0, norm_a = 0.0, norm_b = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            dot_product += a[i] * b[i];
            norm_a += a[i] * a[i];
            norm_b += b[i] * b[i];
        }
        return dot_product / (sqrt(norm_a) * sqrt(norm_b));
    }

// Create a new FileItem
    void FileItem::create_file_item(const UserFileItem &file_item) {
        db << "INSERT INTO file_items (file_id, user_id, sharing, content, local_embedding, openai_embedding, tokens) "
              "VALUES (?, ?, ?, ?, ?, ?, ?);"
           << file_item.file_id
           << file_item.user_id
           << file_item.sharing
           << file_item.content
           << serialize_vector(file_item.local_embedding)   // Serialize to string
           << serialize_vector(file_item.openai_embedding)  // Serialize to string
           << file_item.tokens;
    }

// Get a FileItem by ID
    UserFileItem FileItem::get_file_item_by_id(const int &file_item_id) {
        UserFileItem file_item;

        db
                << "SELECT id, file_id, user_id, sharing, content, local_embedding, openai_embedding, tokens, created_at, updated_at "
                   "FROM file_items WHERE id = ?;"
                << file_item_id
                >> [&](int id, int file_id, std::string user_id, std::string sharing, std::string content,
                       std::string local_embedding_str, std::string openai_embedding_str, int tokens,
                       std::string created_at, std::string updated_at) {
                    file_item.id = id;
                    file_item.file_id = file_id;
                    file_item.user_id = user_id;
                    file_item.sharing = sharing;
                    file_item.content = content;
                    file_item.local_embedding = deserialize_vector(local_embedding_str);  // Deserialize string
                    file_item.openai_embedding = deserialize_vector(openai_embedding_str); // Deserialize string
                    file_item.tokens = tokens;
                    file_item.created_at = created_at;
                    file_item.updated_at = updated_at;
                };

        return file_item;
    }

// Update a FileItem
    bool FileItem::update_file_item(const int &file_item_id, const UserFileItem &file_item) {
        int rows_affected = 0;

        db << "UPDATE file_items SET file_id = ?, user_id = ?, sharing = ?, content = ?, "
              "local_embedding = ?, openai_embedding = ?, tokens = ?, updated_at = CURRENT_TIMESTAMP "
              "WHERE id = ?;"
           << file_item.file_id
           << file_item.user_id
           << file_item.sharing
           << file_item.content
           << serialize_vector(file_item.local_embedding)   // Serialize to string
           << serialize_vector(file_item.openai_embedding)  // Serialize to string
           << file_item.tokens
           << file_item_id;

        db << "SELECT changes();" >> rows_affected;
        return rows_affected > 0;
    }

// Delete a FileItem by ID
    bool FileItem::delete_file_item(const int &file_item_id) {
        int rows_affected = 0;

        db << "DELETE FROM file_items WHERE id = ?;" << file_item_id;

        db << "SELECT changes();" >> rows_affected;
        return rows_affected > 0;
    }

// Convert FileItem to JSON
    void FileItem::to_json(json &j, const UserFileItem &user_file_item) {
        j = json{
                {"id",               user_file_item.id},
                {"file_id",          user_file_item.file_id},
                {"user_id",          user_file_item.user_id},
                {"sharing",          user_file_item.sharing},
                {"content",          user_file_item.content},
                {"local_embedding",  user_file_item.local_embedding},
                {"openai_embedding", user_file_item.openai_embedding},
                {"tokens",           user_file_item.tokens},
                {"created_at",       user_file_item.created_at},
                {"updated_at",       user_file_item.updated_at}};
    }

// Get file items by file ID
    std::vector<UserFileItem> FileItem::file_items_by_file(const int &file_id) {
        std::vector<UserFileItem> file_items;

        db
                << "SELECT id, file_id, user_id, sharing, content, local_embedding, openai_embedding, tokens, created_at, updated_at "
                   "FROM file_items WHERE file_id = ?;"
                << file_id
                >> [&](int id, int file_id, std::string user_id, std::string sharing, std::string content,
                       std::string local_embedding_str, std::string openai_embedding_str, int tokens,
                       std::string created_at, std::string updated_at) {
                    UserFileItem file_item;
                    file_item.id = id;
                    file_item.file_id = file_id;
                    file_item.user_id = user_id;
                    file_item.sharing = sharing;
                    file_item.content = content;
                    file_item.local_embedding = deserialize_vector(local_embedding_str);  // Deserialize string
                    file_item.openai_embedding = deserialize_vector(openai_embedding_str); // Deserialize string
                    file_item.tokens = tokens;
                    file_item.created_at = created_at;
                    file_item.updated_at = updated_at;

                    file_items.push_back(file_item);
                };

        return file_items;
    }

// Match file items locally
    std::vector<UserFileItem>
    FileItem::match_file_items_local(const std::vector<float> &query_embedding, const int &match_count,
                                     const std::vector<int> &file_ids) {
        std::vector<UserFileItem> file_items;

        // Create the SQL query to retrieve all relevant items
        std::string file_ids_placeholder = "(";
        for (size_t i = 0; i < file_ids.size(); ++i) {
            file_ids_placeholder += std::to_string(file_ids[i]);
            if (i != file_ids.size() - 1) {
                file_ids_placeholder += ",";
            }
        }
        file_ids_placeholder += ")";

        // Fetch all matching items
        db <<
           "SELECT id, file_id, user_id, sharing, content, local_embedding, openai_embedding, tokens, created_at, updated_at "
           "FROM file_items WHERE file_id IN " + file_ids_placeholder + ";"
           >> [&](int id, int file_id, std::string user_id, std::string sharing, std::string content,
                  std::string local_embedding_str, std::string openai_embedding_str, int tokens,
                  std::string created_at, std::string updated_at) {
               UserFileItem file_item;
               file_item.id = id;
               file_item.file_id = file_id;
               file_item.user_id = user_id;
               file_item.sharing = sharing;
               file_item.content = content;
               file_item.local_embedding = deserialize_vector(local_embedding_str);  // Deserialize string
               file_item.openai_embedding = deserialize_vector(openai_embedding_str); // Deserialize string
               file_item.tokens = tokens;
               file_item.created_at = created_at;
               file_item.updated_at = updated_at;

               file_items.push_back(file_item);
           };

        // Sort by similarity to the query_embedding
        std::sort(file_items.begin(), file_items.end(), [&](const UserFileItem &a, const UserFileItem &b) {
            float distance_a = calculate_cosine_similarity(a.local_embedding, query_embedding);
            float distance_b = calculate_cosine_similarity(b.local_embedding, query_embedding);
            return distance_a > distance_b; // Sort descending by similarity
        });

        // Return only the top match_count items
        if (file_items.size() > match_count) {
            file_items.resize(match_count);
        }

        return file_items;
    }


}
