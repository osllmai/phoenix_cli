#include "api/include/controllers/streaming_controller.h"
#include <crow.h>
#include <sstream>
#include <vector>
#include <string>

namespace controllers {
    std::string create_chunk(const std::string &data) {
        std::stringstream ss;
        ss << std::hex << data.length() << "\r\n" << data << "\r\n";
        return ss.str();
    }

    void stream_tokens(const crow::request &, crow::response &res) {
        res.set_header("Transfer-Encoding", "chunked");
        res.set_header("Content-Type", "application/json");

        auto chunked_write = [&res](const std::string &data) {
            res.write(create_chunk(data));
        };

        // Example tokens to stream
        std::vector<std::string> tokens = {
                "{\"token\": \"token1\"}",
                "{\"token\": \"token2\"}",
                "{\"token\": \"token3\"}"
                // Add more tokens as needed
        };

        // Stream each token
        for (const auto &token: tokens) {
            chunked_write(token);
        }

        // Send the final zero-length chunk to signify the end
        res.write("0\r\n\r\n");
        res.end();
    }
}
