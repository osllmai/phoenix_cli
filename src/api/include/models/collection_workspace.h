#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UserCollectionWorkspace {
    std::string user_id;
    int collection_id;
    int workspace_id;
    std::string created_at;
    std::string updated_at;
};

namespace models {
    class CollectionWorkspace {
    public:
        static UserCollectionWorkspace get_collection_workspace_by_id(const int &collection_id, const int &workspace_id);
        static void create_collection_workspace(const UserCollectionWorkspace &collection_workspace);
        static bool update_collection_workspace(const int &collection_id, const int &workspace_id, const UserCollectionWorkspace &collection_workspace);
        static bool delete_collection_workspace(const int &collection_id, const int &workspace_id);
        static std::vector<UserCollectionWorkspace> collection_workspaces(const std::string &user_id);
        static void to_json(json &j, const UserCollectionWorkspace &collection_workspace);
    };
}
