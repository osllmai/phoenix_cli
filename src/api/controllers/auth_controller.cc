#include "api/include/controllers/auth_controller.h"
#include "api/include/models/user.h"
#include "api/include/models/profile.h"
#include "api/include/utils/utils.h"

#include "sqlite_modern_cpp/errors.h"
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>
#include <crow.h>
#include <cstdlib>
#include <ctime>
#include <string>

using json = nlohmann::json;

namespace controllers {
    std::string generate_random_number(int length = 10) {
        std::srand(std::time(0));
        std::string username_number;
        username_number += std::to_string((std::rand() % 9) + 1);

        // Generate the remaining 9 digits
        for (int i = 1; i < length; ++i) {
            username_number += std::to_string(std::rand() % 10);
        }

        return username_number;
    }

    crow::response register_user(const crow::request &req) {
        try {
            auto user = json::parse(req.body);
            if (user["email"].empty() || user["password"].empty()) {
                return crow::response(400, "Email and password are required");
            }

            int user_id = models::User::create(user["email"].get<std::string>(), user["password"].get<std::string>());
            std::string username = "user" + generate_random_number();
            UserProfile user_profile(
                    std::to_string(user_id),
                    user.value("bio", ""),
                    user.value("image_url", ""),
                    user.value("image_path", ""),
                    user.value("profile_context", ""),
                    user.value("display_name", ""),
                    user.value("use_azure_openai", false),
                    user.value("username", username)
            );

            models::Profile::create(user_profile);
            return crow::response(201, "User created successfully");
        } catch (const json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "Invalid JSON: " + std::string(e.what()));
        } catch (const sqlite::sqlite_exception &e) {
            CROW_LOG_ERROR << "Database error: " << e.what();
            return crow::response(500, "Database error: " + std::string(e.what()));
        } catch (const std::exception &e) {
            CROW_LOG_ERROR << "Error creating user: " << e.what();
            return crow::response(500, "Error creating user: " + std::string(e.what()));
        }
    }

    crow::response login(const crow::request &req) {
        try {
            auto credentials = json::parse(req.body);
            std::string email = credentials["email"];
            std::string password = credentials["password"];

            if (email.empty() || password.empty()) {
                return crow::response(400, "Email and password are required");
            }
            auto user = models::User::get_user_by_email(email);
            if (models::User::authenticate(email, password)) {
                std::string access_token = create_jwt(user.id, false);
                std::string refresh_token = create_jwt(user.id, true);
                json response = {
                        {"access_token",  access_token},
                        {"refresh_token", refresh_token},
                };
                return crow::response(200, response.dump());
            } else {
                return crow::response(401, "Invalid credentials");
            }
        } catch (const json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "Invalid JSON: " + std::string(e.what()));
        } catch (const std::exception &e) {
            CROW_LOG_ERROR << "Error during login: " << e.what();
            return crow::response(500, "Error during login: " + std::string(e.what()));
        }
    }

    crow::response refresh(const crow::request &req) {
        try {
            auto body = json::parse(req.body);
            std::string refresh_token = body["refresh_token"];

            if (verify_jwt(refresh_token)) {
                auto decoded = jwt::decode(refresh_token);
                auto token_type = decoded.get_payload_claim("token_type").as_string();

                if (token_type != "refresh") {
                    return crow::response(400, "Invalid token type");
                }

                auto user_id = decoded.get_payload_claim("user_id").as_integer();
                // false to create access_token
                std::string new_access_token = create_jwt(user_id, false);

                json response = {
                        {"access_token", new_access_token}
                };

                return crow::response(200, response.dump());
            } else {
                return crow::response(401, "Invalid refresh token");
            }
        } catch (const json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "Invalid JSON: " + std::string(e.what()));
        } catch (const std::exception &e) {
            CROW_LOG_ERROR << "Error during refresh token: " << e.what();
            return crow::response(400, "Error during refresh token: " + std::string(e.what()));
        }
    }

    crow::response reset_password(const crow::request &req) {
        try {
            auto body = json::parse(req.body);
            std::string email = body["email"];
            std::string old_password = body["old_password"];
            std::string new_password = body["new_password"];

            if (email.empty() || old_password.empty() || new_password.empty()) {
                return crow::response(422, "email, old_password and new_password is required");
            }

            if (models::User::authenticate(email, old_password)) {
                if (models::User::reset_password(email, old_password, new_password)) {
                    return crow::response(200, "Your password changed successfully");
                } else {
                    return crow::response(400, "Failed to change password");
                }
            } else {
                return crow::response(403, "Invalid credentials");
            }
        } catch (const json::exception &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "JSON parsing error: " + std::string(e.what()));
        } catch (const std::exception &e) {
            CROW_LOG_ERROR << "During change password: " << e.what();
            return crow::response(400, "During change password: " + std::string(e.what()));
        }
    }

    crow::response protected_route(const crow::request &req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return crow::response(401, "No Authorization header provided");
        }

        if (verify_jwt(auth_header)) {
            auto user_id_opt = get_user_id_from_token(auth_header);
            if (user_id_opt) {
                json response = {
                        {"user_id", *user_id_opt}
                };
                return crow::response(200, response.dump());
            } else {
                return crow::response(500, "Error extracting user_id from token");
            }
        } else {
            return crow::response(401, "Unauthorized");
        }
    }
}
