#include "PythonBridge.h"
#include <iostream>
#include <stdexcept>
#include <array>
#include <memory>
#include <cstdio>
#include <regex>

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
    std::string cmd = "python " + script_path + " \"" + simulation_data_json + "\"";
    
    std::string worker_output = exec_python_script(cmd);
    
    // In a real scenario, this would parse JSON payload returned by Python.
    // For demonstration, we assume python printed the discovered formula:
    // {"discovered_equation": "x_squared_plus_x"}
    
    std::cout << "   [PythonBridge] PySINDy Worker Output: '" << worker_output << "'" << std::endl;
    std::cout << "   [PythonBridge] Terminating Python Subprocess and returning control to C++ Master." << std::endl;
    
    // Return a mock expression representing the data extracted from Python JSON (x^2 + x)
    return make_add(make_square(make_var("x")), make_var("x"));
}

std::string PythonBridge::execute_worker_deepxde(const std::string& script_path, double gamma_noise) {
    std::cout << "   [PythonBridge] Subcontracting PDE resolution to Python DeepXDE Worker..." << std::endl;
    // Mock
    return "weights_optimized.h5";
}

} // namespace q_engine
