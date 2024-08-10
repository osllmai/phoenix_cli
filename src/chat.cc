#include "header.h"
#include "llmodel.h"
#include "utils.h"
#include "parse_json.h"
#include "chat_manager.h"
#include "web_server.h"

#include <string>
#include <atomic>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

//////////////////////////////////////////////////////////////////////////
////////////                    ANIMATION                     ////////////
//////////////////////////////////////////////////////////////////////////

std::atomic<bool> stop_display{false};

// Function to display animation frames
void display_frames() {
    const char *frames[] = {".", ":", "'", ":"};
    int frame_index = 0;
    ConsoleState con_st;
    con_st.use_color = true;
    while (!stop_display) {
        set_console_color(con_st, PROMPT);
        std::cerr << "\r" << frames[frame_index % 4] << std::flush;
        frame_index++;
        set_console_color(con_st, DEFAULT);
        if (!stop_display) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::cerr << "\r" << " " << std::flush;
            std::cerr << "\r" << std::flush;
        }
    }
}

// Function to display loading animation
void display_loading() {
    while (!stop_display) {
        for (int i = 0; i < 14; i++) {
            fprintf(stdout, ".");
            fflush(stdout);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (stop_display) { break; }
        }
        std::cout << "\r" << "               " << "\r" << std::flush;
    }
    std::cout << "\r" << " " << std::flush;
}

//////////////////////////////////////////////////////////////////////////
////////////                   /ANIMATION                     ////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
////////////                 CHAT FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////

// Function to get user input
std::string get_input(ConsoleState &con_st, std::string &input, chatParams &params) {
    set_console_color(con_st, USER_INPUT);
    std::cout << "\n> ";
    std::getline(std::cin, input);

    std::istringstream iss(input);
    std::string input1, input2;
    std::getline(iss, input1, ' ');
    std::getline(iss, input2, ' ');
    set_console_color(con_st, DEFAULT);
    return input;
}

std::string hashstring = "";
std::string answer = "";

//////////////////////////////////////////////////////////////////////////
////////////                /CHAT FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////

int run_command(const std::string &model_path) {
    ConsoleState con_st;
    con_st.use_color = true;
    set_console_color(con_st, DEFAULT);

    set_console_color(con_st, PROMPT);
    set_console_color(con_st, BOLD);
    std::cout << APPNAME;
    set_console_color(con_st, DEFAULT);
    set_console_color(con_st, PROMPT);
    std::cout << " (v. " << VERSION << ")\n\n";

    chatParams params;
    params.model = model_path;

    LLModel::PromptContext prompt_context;
    prompt_context.n_ctx = 2048;
    int ngl = 100;
    LLModel *model = LLModel::Implementation::construct(model_path, "auto", prompt_context.n_ctx);
    std::string prompt_template = "<|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n%2<|eot_id|>";

#if(WIN32)
    if (params.backend == "cuda") {
        auto devices = LLModel::Implementation::availableGPUDevices();
        if (!devices.empty()) {
            for (const auto& device : devices) {
                size_t memoryRequired = devices[0].heapSize;
                const std::string& name = devices[0].name;
                const size_t requiredMemory = model->requiredMem(model_path, prompt_context.n_ctx, ngl);
                auto devices = model->availableGPUDevices(memoryRequired);
                for (const auto& device : devices) {
                    if (device.name == name && model->initializeGPUDevice(device.index)) {
                        break;
                    }
                }
            }
        }
    }
#endif

    std::future<void> future;
    stop_display = true;

    if (params.use_animation) {
        stop_display = false;
        future = std::async(std::launch::async, display_loading);
    }

    auto check_model = model->loadModel(model_path, prompt_context.n_ctx, ngl);
    if (!check_model) {
        if (params.use_animation) {
            stop_display = true;
            future.wait();
            stop_display = false;
        }

        std::cerr << "Error loading: " << params.model.c_str() << std::endl;
        std::cout << "Press any key to exit..." << std::endl;
        std::cin.get();
        return 0;
    } else {
        if (params.use_animation) {
            stop_display = true;
            future.wait();
        }
        std::cout << "\r" << APPNAME << ": done loading!" << std::flush;
    }

    set_console_color(con_st, DEFAULT);

    //////////////////////////////////////////////////////////////////////////
    ////////////            PROMPT LAMBDA FUNCTIONS               ////////////
    //////////////////////////////////////////////////////////////////////////

    auto prompt_callback = [](int32_t token_id) {
        // You can handle prompt here if needed
        return true;
    };

    auto response_callback = [](int32_t token_id, const std::string responsechars_str) {
        const char *responsechars = responsechars_str.c_str();

        if (!(responsechars == nullptr || responsechars[0] == '\0')) {
            // Stop the animation, printing response
            if (!stop_display) {
                stop_display = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                std::cerr << "\r" << " " << std::flush;
                std::cerr << "\r" << std::flush;
                if (!answer.empty()) { std::cout << answer; }
            }

            std::cout << responsechars << std::flush;
            answer += responsechars;
        }

        return true;
    };

    auto recalculate_callback = [](bool is_recalculating) {
        // You can handle recalculation requests here if needed
        return is_recalculating;
    };

    //////////////////////////////////////////////////////////////////////////
    ////////////         PROMPT TEXT AND GET RESPONSE             ////////////
    //////////////////////////////////////////////////////////////////////////
    model->setThreadCount(8);
    std::string input = "";

    // Main chat loop
    while (true) {
        input = get_input(con_st, input, params);

        std::string unique_identifier = ChatManager::generate_unique_id();
        ChatManager::create_chat_config_file(unique_identifier, params);

        if (params.use_animation) {
            stop_display = false;
            future = std::async(std::launch::async, display_frames);
        }
        if (!params.b_token.empty()) {
            answer += params.b_token;
            if (!params.use_animation) { std::cout << params.b_token; }
        }
        model->prompt(input, prompt_template,
                      prompt_callback, response_callback, recalculate_callback, prompt_context, false, nullptr);

        if (!params.e_token.empty()) {
            std::cout << params.e_token;
            answer += params.e_token;
        }
        if (params.use_animation) {
            stop_display = true;
            future.wait();
            stop_display = false;
        }
        if (!params.save_log.empty()) {
            save_chat_log(params.save_log, (params.default_prefix + params.default_header + input +
                                            params.default_footer).c_str(), answer.c_str());
        }

        ChatManager::save_chat_history(unique_identifier, input.c_str(), answer.c_str());
        answer.clear(); // Clear the answer for the next prompt
    }

    set_console_color(con_st, DEFAULT);
    return 0;
}


std::string chat_with_api(const std::string &model_path, const std::string &prompt) {
    ConsoleState con_st;
    con_st.use_color = true;
    set_console_color(con_st, DEFAULT);

    set_console_color(con_st, PROMPT);
    set_console_color(con_st, BOLD);
    std::cout << APPNAME;
    set_console_color(con_st, DEFAULT);
    set_console_color(con_st, PROMPT);
    std::cout << " (v. " << VERSION << ")\n\n";

    chatParams params;
    params.model = model_path;

    LLModel::PromptContext prompt_context;
    prompt_context.n_ctx = 2048;
    int ngl = 100;
    LLModel *model = LLModel::Implementation::construct(model_path, "auto", prompt_context.n_ctx);
    std::string prompt_template = "<|start_header_id|>user<|end_header_id|>\n\n%1<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n%2<|eot_id|>";

#if(WIN32)
    if (params.backend == "cuda") {
        auto devices = LLModel::Implementation::availableGPUDevices();
        if (!devices.empty()) {
            for (const auto& device : devices) {
                size_t memoryRequired = devices[0].heapSize;
                const std::string& name = devices[0].name;
                const size_t requiredMemory = model->requiredMem(model_path, prompt_context.n_ctx, ngl);
                auto devices = model->availableGPUDevices(memoryRequired);
                for (const auto& device : devices) {
                    if (device.name == name && model->initializeGPUDevice(device.index)) {
                        break;
                    }
                }
            }
        }
    }
#endif

    std::future<void> future;
    stop_display = true;

    if (params.use_animation) {
        stop_display = false;
        future = std::async(std::launch::async, display_loading);
    }

    auto check_model = model->loadModel(model_path, prompt_context.n_ctx, ngl);
    if (!check_model) {
        if (params.use_animation) {
            stop_display = true;
            future.wait();
            stop_display = false;
        }

        std::cerr << "Error loading: " << params.model.c_str() << std::endl;
        std::cout << "Press any key to exit..." << std::endl;
        std::cin.get();
        return "";
    } else {
        if (params.use_animation) {
            stop_display = true;
            future.wait();
        }
        std::cout << "\r" << APPNAME << ": done loading!" << std::flush;
    }

    set_console_color(con_st, DEFAULT);

    //////////////////////////////////////////////////////////////////////////
    ////////////            PROMPT LAMBDA FUNCTIONS               ////////////
    //////////////////////////////////////////////////////////////////////////

    auto prompt_callback = [](int32_t token_id) {
        // You can handle prompt here if needed
        return true;
    };

    auto response_callback = [](int32_t token_id, const std::string responsechars_str) {
        const char *responsechars = responsechars_str.c_str();

        if (!(responsechars == nullptr || responsechars[0] == '\0')) {
            // Stop the animation, printing response
            if (!stop_display) {
                stop_display = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                std::cerr << "\r" << " " << std::flush;
                std::cerr << "\r" << std::flush;
                if (!answer.empty()) { std::cout << answer; }
            }

            std::cout << responsechars << std::flush;
            answer += responsechars;
        }

        return true;
    };

    auto recalculate_callback = [](bool is_recalculating) {
        // You can handle recalculation requests here if needed
        return is_recalculating;
    };

    //////////////////////////////////////////////////////////////////////////
    ////////////         PROMPT TEXT AND GET RESPONSE             ////////////
    //////////////////////////////////////////////////////////////////////////
    model->setThreadCount(8);

    std::string unique_identifier = ChatManager::generate_unique_id();
    ChatManager::create_chat_config_file(unique_identifier, params);

    if (params.use_animation) {
        stop_display = false;
        future = std::async(std::launch::async, display_frames);
    }
    if (!params.b_token.empty()) {
        answer += params.b_token;
        if (!params.use_animation) { std::cout << params.b_token; }
    }
    model->prompt(prompt, prompt_template,
                  prompt_callback, response_callback, recalculate_callback, prompt_context, false, nullptr);

    if (!params.e_token.empty()) {
        std::cout << params.e_token;
        answer += params.e_token;
    }
    if (params.use_animation) {
        stop_display = true;
        future.wait();
        stop_display = false;
    }
    if (!params.save_log.empty()) {
        save_chat_log(params.save_log, (params.default_prefix + params.default_header + prompt +
                                        params.default_footer).c_str(), answer.c_str());
    }

    ChatManager::save_chat_history(unique_identifier, prompt.c_str(), answer.c_str());
    std::string response = answer;
    answer.clear(); // Clear the answer for the next prompt

//    std::cout << "AI >>> " << response << std::endl;

    set_console_color(con_st, DEFAULT);
    return response;
}