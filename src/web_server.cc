#include "web_server.h"

#include <iostream>
#include <thread>


// Function to handle non-streaming requests
std::string handle_api_generate(http::request<http::string_body> const& req, http::response<http::string_body>& res) {
    try {
        // Parse the JSON body
        json json_data = json::parse(req.body());

        // Extract the model and prompt
        std::string model = json_data["model"];
        std::string prompt = json_data["prompt"];

        // Process the request
        std::string message = "Received model: " + model + ", prompt: " + prompt;

        // Set the response
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        json response_json = {
                {"message", message}
        };
        res.body() = response_json.dump();
        res.prepare_payload();

        return prompt;
    } catch (const std::exception& e) {
        res.result(http::status::bad_request);
        res.set(http::field::content_type, "application/json");
        json error_response = {
                {"error", "Invalid JSON data"}
        };
        res.body() = error_response.dump();
        res.prepare_payload();
        return "";
    }
}

std::string handle_request(tcp::socket& socket, beast::flat_buffer& buffer) {
    try {
        http::request<http::string_body> req;
        http::response<http::string_body> res;

        // Read the request
        http::read(socket, buffer, req);

        // Check if the request is a POST to /api/generate
        if (req.method() == http::verb::post && req.target() == "/api/generate") {
            std::string message = handle_api_generate(req, res);

            // Process the request and get the response from the API
//            std::string model_name = "/Users/amir/Workspace/models/Meta-Llama-3-8B-Instruct.Q4_0.gguf";
//            std::string api_response = chat_with_api(model_name, message);

            // Send the API response back to the client
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            json response_json = {
                    {"answer", message}
            };
            res.body() = response_json.dump();
            res.prepare_payload();
            http::write(socket, res);

            return message;
        } else {
            res.result(http::status::not_found);
            res.set(http::field::content_type, "application/json");
            json error_response = {
                    {"error", "Resource not found."}
            };
            res.body() = error_response.dump();
            res.prepare_payload();
            http::write(socket, res);
            return "";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling request: " << e.what() << std::endl;
        return "";
    }
}

//std::string handle_streaming_request(http::request<http::string_body> const& req, http::response<http::string_body>& res, tcp::socket& socket) {
//    // Simulate chatbot response in chunks
//    json response1 = {
//            {"message", "This is the first part of the streaming response."}
//    };
//    json response2 = {
//            {"message", "This is the second part of the streaming response."}
//    };
//
//    res.result(http::status::ok);
//    res.set(http::field::content_type, "application/json");
//    res.set(http::field::transfer_encoding, "chunked");
//    res.body() = response1.dump();
//    res.prepare_payload();
//
//    // Send the first part of the response
//    http::write(socket, res);
//
//    // Simulate delay for streaming
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//
//    // Send the second part of the response
//    res.body() = response2.dump();
//    res.prepare_payload();
//    http::write(socket, res);
//
//    return response2["message"];
//}

//std::string handle_request(tcp::socket& socket, beast::flat_buffer& buffer) {
//    try {
//        http::request<http::string_body> req;
//        http::response<http::string_body> res;
//
//        // Read the request
//        http::read(socket, buffer, req);
//
//        // Determine the request type and handle accordingly
//        if (req.target() == "/api/generate") {
//            std::string message = handle_api_generate(req, res);
//            http::write(socket, res);
//            return message;
//        } else {
//            res.result(http::status::not_found);
//            res.set(http::field::content_type, "application/json");
//            json error_response = {
//                    {"error", "Resource not found."}
//            };
//            res.body() = error_response.dump();
//            res.prepare_payload();
//            http::write(socket, res);
//            return "";
//        }
//    } catch (const std::exception& e) {
//        std::cerr << "Error handling request: " << e.what() << std::endl;
//        return "";
//    }
//}