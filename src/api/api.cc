#include "api/include/crow_all.h"
#include "api/include/routes/auth.h"
#include "api/include/routes/streaming.h"
#include "api/include/models/database.h"

void endpoints() {
    crow::SimpleApp app;

    // Initialize the database
    models::init_db();

    // Set the log level
    app.loglevel(crow::LogLevel::Debug);

    // Initialize routes
    routes::init_auth_routes(app);
    routes::init_streaming_routes(app);

    // Start the server
    app.port(18080).multithreaded().run();
}