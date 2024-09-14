#include "api/include/crow_all.h"
#include "api/include/routes/auth.h"
#include "api/include/routes/profile.h"
#include "api/include/routes/streaming.h"
#include "api/include/routes/workspace.h"
#include "api/include/routes/folder.h"
#include "api/include/routes/file.h"
#include "api/include/routes/file_item.h"
#include "api/include/routes/preset.h"
#include "api/include/routes/preset_workspace.h"
#include "api/include/routes/assistant.h"
#include "api/include/routes/assistant_workspace.h"
#include "api/include/routes/chat.h"
#include "api/include/routes/chat_file.h"
#include "api/include/routes/message.h"
#include "api/include/routes/message_file_item.h"
#include "api/include/routes/prompt.h"
#include "api/include/routes/prompt_workspace.h"
#include "api/include/routes/assistant_collection.h"
#include "api/include/routes/assistant_file.h"
#include "api/include/routes/assistant_tool.h"
#include "api/include/routes/collection.h"
#include "api/include/routes/collection_file.h"
#include "api/include/routes/collection_workspace.h"
#include "api/include/routes/model.h"
#include "api/include/routes/model_workspace.h"
#include "api/include/routes/tool.h"
#include "api/include/routes/tool_workspace.h"
#include "api/include/models/database.h"
#include <crow/middlewares/cors.h>

void endpoints() {
    crow::App<crow::CORSHandler> app;

    auto &cors = app.get_middleware<crow::CORSHandler>();
    cors
    .global()
    .headers("Content-Type", "Authorization")
    .methods("POST"_method, "GET"_method, "PUT"_method, "DELETE"_method, "PATCH"_method)
    .origin("http://localhost:3001");



    // Initialize the database
    models::init_db();

    // Set the log level
    app.loglevel(crow::LogLevel::Debug);

    // Initialize routes
    routes::init_auth_routes(app);
    routes::init_streaming_routes(app);
    routes::init_profile_routes(app);
    routes::init_workspace_routes(app);
    routes::init_folder_routes(app);
    routes::init_file_routes(app);
    routes::init_file_item_routes(app);
    routes::init_preset_routes(app);
    routes::init_preset_workspace_routes(app);
    routes::init_assistant_routes(app);
    routes::init_assistant_workspace_routes(app);
    routes::init_chat_routes(app);
    routes::init_chat_file_routes(app);
    routes::init_message_routes(app);
    routes::init_message_file_item_routes(app);
    routes::init_prompt_routes(app);
    routes::init_prompt_workspace_routes(app);
    routes::init_assistant_collection_routes(app);
    routes::init_assistant_file_routes(app);
    routes::init_assistant_tool_routes(app);
    routes::init_collection_routes(app);
    routes::init_collection_file_routes(app);
    routes::init_collection_workspace_routes(app);
    routes::init_tool_routes(app);
    routes::init_tool_workspace_routes(app);
    routes::init_model_routes(app);
    routes::init_model_workspace_routes(app);

    // Start the server
    app.port(18080).multithreaded().run();
}
