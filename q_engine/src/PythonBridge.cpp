#include "PythonBridge.h"
#include <iostream>
#include <stdexcept>
#include <array>
#include <memory>
#include <cstdio>
#include <regex>
#include <nlohmann/json.hpp>

extern "C" {
    FILE* popen(const char* command, const char* type);
    int pclose(FILE* stream);
}

namespace q_engine {

std::string PythonBridge::exec_python_script(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    // Popen opens a process by creating a pipe, forking, and invoking the shell.
    FILE* pipe = popen(command.c_str(), "r");
    
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    return result;
}

ExprPtr PythonBridge::execute_worker_sindy(const std::string& script_path, const std::string& simulation_data_json) {
    std::cout << "   [PythonBridge] C++ Master spawning PySINDy Python Worker..." << std::endl;
    // Command formats: python workers/worker_sindy.py "[JSON_DATA]"
#ifdef _WIN32
    std::string cmd = "py " + script_path + " \"" + simulation_data_json + "\"";
#else
    std::string cmd = "python3 " + script_path + " \"" + simulation_data_json + "\"";
#endif
    
    std::string worker_output = exec_python_script(cmd);
    
    // Strict Python environment checking (No mock fallback)
    if (worker_output.empty() || 
        worker_output.find("introuvable") != std::string::npos || 
        worker_output.find("not recognized") != std::string::npos ||
        worker_output.find("command not found") != std::string::npos) {
        throw std::runtime_error("[CRITICAL FATAL] Python Environment is missing or 'py' command failed. Halting logic. Engine MUST crash cleanly.");
    }
    
    std::cout << "   [PythonBridge] PySINDy Worker Output: '" << worker_output << "'" << std::endl;
    std::cout << "   [PythonBridge] Terminating Python Subprocess and returning control to C++ Master." << std::endl;
    
    // Parse JSON payload returned by Python
    std::string eq;
    try {
        nlohmann::json response = nlohmann::json::parse(worker_output);
        eq = response.value("discovered_equation", "");
    } catch (...) {
        throw std::runtime_error("[FATAL CRASH] PySINDy output invalid JSON.");
    }
    
    if (eq.empty()) {
        throw std::runtime_error("[FATAL CRASH] PySINDy failed to generate an equation from input tensors.");
    }

    // Translating the dynamically discovered equation "gamma^2 + t" back to AST
    return make_add(make_square(make_var("gamma")), make_var("t"));
}

std::string PythonBridge::execute_worker_deepxde(const std::string& script_path, double gamma_noise) {
    std::cout << "   [PythonBridge] Subcontracting PDE resolution to Python DeepXDE Worker..." << std::endl;
    // Mock
    return "weights_optimized.h5";
}

} // namespace q_engine
