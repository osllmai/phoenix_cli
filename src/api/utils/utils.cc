#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <crow.h>
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


std::string get_current_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&currentTime);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::optional<std::string> get_user_id_from_token(const std::string &token) {
    try {
        auto decoded = jwt::decode(token);
        auto user_id_claim = decoded.get_payload_claim("user_id");

        if (user_id_claim.get_type() == jwt::json::type::integer) {
            return user_id_claim.as_string();
        } else if (user_id_claim.get_type() == jwt::json::type::string) {
            return user_id_claim.as_string();
        } else {
            CROW_LOG_ERROR << "Invalid user_id type in JWT";
            return std::nullopt;
        }
    } catch (const std::exception &e) {
        CROW_LOG_ERROR << "Failed to get user_id from JWT: " << e.what();
        return std::nullopt;
    }
}

const std::string jwt_secret = "your_secret_key";

std::string create_jwt(const int &user_id, bool is_refresh_token = false) {
    auto now = std::chrono::system_clock::now();
    auto exp = now + (is_refresh_token ? std::chrono::days(30) : std::chrono::hours(6));
    std::string token_type = is_refresh_token ? "refresh" : "access";

    auto token = jwt::create()
            .set_issuer("auth0")
            .set_type("JWS")
            .set_payload_claim("user_id", jwt::claim(std::to_string(user_id)))
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

        auto user_id = decoded.get_payload_claim("user_id");
        return true;
    } catch (const std::exception &e) {
        CROW_LOG_ERROR << "JWT verification failed: " << e.what();
        return false;
    }
}

// Serialize the vector of image paths to a JSON string
std::string serialize_image_paths(const std::vector<std::string>& image_paths) {
    return json(image_paths).dump();
}

// Deserialize the JSON string to a vector of image paths
std::vector<std::string> deserialize_image_paths(const std::string& image_paths_str) {
    return json::parse(image_paths_str).get<std::vector<std::string>>();
}
