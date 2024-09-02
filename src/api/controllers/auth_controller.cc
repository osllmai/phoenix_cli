#include "api/include/controllers/auth_controller.h"
#include "api/include/models/user.h"
#include "sqlite_modern_cpp/errors.h"
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>
#include <crow.h>

using json = nlohmann::json;

namespace controllers {
    const std::string jwt_secret = "your_secret_key";

    std::string create_jwt(const std::string &email, bool is_refresh_token = false) {
        auto now = std::chrono::system_clock::now();
        auto exp = now + (is_refresh_token ? std::chrono::days(30) : std::chrono::hours(6));
        std::string token_type = is_refresh_token ? "refresh" : "access";

        auto token = jwt::create()
                .set_issuer("auth0")
                .set_type("JWS")
                .set_payload_claim("email", jwt::claim(email))
                .set_payload_claim("token_type", jwt::claim(token_type))
                .set_issued_at(now)
                .set_expires_at(exp)
                .sign(jwt::algorithm::hs256{jwt_secret});
        return token;
    }

    bool verify_jwt(const std::string &token) {
        try {
            auto decoded = jwt::decode(token);

            auto exp = decoded.get_expires_at();
            auto now = std::chrono::system_clock::now();
            if (exp < now) {
                CROW_LOG_ERROR << "JWT has expired";
                return false;
            }

            jwt::verify()
                    .allow_algorithm(jwt::algorithm::hs256{jwt_secret})
                    .with_issuer("auth0")
                    .verify(decoded);
            return true;
        } catch (const std::exception &e) {
            CROW_LOG_ERROR << "JWT verification failed: " << e.what();
            return false;
        }
    }

    crow::response register_user(const crow::request &req) {
        try {
            auto user = json::parse(req.body);
            if (user["email"].empty() || user["password"].empty()) {
                return crow::response(400, "Email and password are required");
            }

            models::User::create(user["email"].get<std::string>(), user["password"].get<std::string>());
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

            if (models::User::authenticate(email, password)) {
                std::string access_token = create_jwt(email, false);
                std::string refresh_token = create_jwt(email, true);
                json response = {
                        {"access_token", access_token},
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

                auto email = decoded.get_payload_claim("email").as_string();
                // false to create access_token
                std::string new_access_token = create_jwt(email, false);

                json response = {
                        {"access_token", new_access_token}
                };

                return crow::response(200, response.dump());
            } else {
                return crow::response(401, "Invalid refresh token");
            }
        } catch (const json::exception  &e) {
            CROW_LOG_ERROR << "JSON parsing error: " << e.what();
            return crow::response(400, "Invalid JSON: " + std::string(e.what()));
        } catch(const std::exception &e) {
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

            if(models::User::authenticate(email, old_password)) {
                if(models::User::reset_password(email, old_password, new_password)) {
                    return crow::response(200, "Your password changed successfully");
                } else {
                    return crow::response(400, "Failed to change password");
                }
            } else {
                return crow::response(403, "Invalid credentials");
            }
        } catch (const json::exception  &e) {
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
            return crow::response(200, "This is a protected route");
        } else {
            return crow::response(401, "Unauthorized");
        }
    }
}
