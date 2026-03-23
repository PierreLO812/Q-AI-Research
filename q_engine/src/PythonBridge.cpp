#include "PythonBridge.h"
#include <iostream>
#include <stdexcept>
#include <array>
#include <memory>
#include <cstdio>
#include <cctype>
#include <nlohmann/json.hpp>

extern "C" {
    FILE* popen(const char* command, const char* type);
    int pclose(FILE* stream);
}

namespace q_engine {

// ─── Numeric token checker ────────────────────────────────────────────────────
static bool is_number(const std::string& s) {
    if (s.empty()) return false;
    size_t i = (s[0] == '-') ? 1 : 0;
    bool has_dot = false;
    for (; i < s.size(); ++i) {
        if (s[i] == '.') { if (has_dot) return false; has_dot = true; }
        else if (!std::isdigit((unsigned char)s[i])) return false;
    }
    return (s[0] == '-') ? s.size() > 1 : !s.empty();
}

// ─── Recursive math-string → AST parser ──────────────────────────────────────
// Handles: numeric constants, gamma, t, sigma_x/y/z, '*', '+', '-'
// Supports negative-prefixed coefficients like "-0.01 * gamma * t"
static ExprPtr parse_math_token(std::string eq) {
    // Trim spaces
    eq.erase(0, eq.find_first_not_of(" "));
    if (!eq.empty()) eq.erase(eq.find_last_not_of(" ") + 1);

    // Addition/Subtraction — scan right-to-left so we get left-associativity
    for (int i = (int)eq.size() - 1; i >= 1; --i) {
        char c = eq[i], prev = eq[i-1];
        if (c == '+' && prev != 'e' && prev != 'E') {
            return make_add(parse_math_token(eq.substr(0, i)),
                            parse_math_token(eq.substr(i + 1)));
        }
        // Subtraction but NOT a leading minus (position > 0) and not after 'e'/'E'
        if (c == '-' && prev != 'e' && prev != 'E' && prev != '*') {
            return make_add(parse_math_token(eq.substr(0, i)),
                            make_mul(make_const(-1.0),
                                     parse_math_token(eq.substr(i + 1))));
        }
    }

    // Multiplication — right-to-left
    for (int i = (int)eq.size() - 1; i >= 0; --i) {
        if (eq[i] == '*') {
            return make_mul(parse_math_token(eq.substr(0, i)),
                            parse_math_token(eq.substr(i + 1)));
        }
    }

    // Atoms
    if (eq == "gamma^2") return make_square(make_var("gamma"));
    if (eq == "gamma")   return make_var("gamma");
    if (eq == "t")       return make_var("t");
    if (eq == "x")       return make_var("x");
    if (eq == "sigma_x") return make_pauli(PauliAxis::X);
    if (eq == "sigma_y") return make_pauli(PauliAxis::Y);
    if (eq == "sigma_z") return make_pauli(PauliAxis::Z);
    if (is_number(eq))   return make_const(std::stod(eq));

    throw std::runtime_error("[FATAL CRASH] C++ Parser could not interpret PySINDy token: '" + eq + "'");
}

// ─── Generic python subprocess runner ────────────────────────────────────────
std::string PythonBridge::exec_python_script(const std::string& command) {
    std::array<char, 256> buffer;
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) result += buffer.data();
    pclose(pipe);
    return result;
}

// ─── Main PySINDy worker dispatch ────────────────────────────────────────────
ExprPtr PythonBridge::execute_worker_sindy(const std::string& script_path,
                                           const std::string& simulation_data_json) {
    std::cout << "   [PythonBridge] C++ Master spawning PySINDy Python Worker..." << std::endl;

    // Escape inner quotes for Windows CMD
    std::string escaped;
    for (char c : simulation_data_json) {
        if (c == '"') escaped += "\\\"";
        else escaped += c;
    }
#ifdef _WIN32
    std::string cmd = "py " + script_path + " \"" + escaped + "\"";
#else
    std::string cmd = "python3 " + script_path + " '" + simulation_data_json + "'";
#endif

    std::string out = exec_python_script(cmd);

    // Strict environment check — no silent fallback
    if (out.empty() ||
        out.find("introuvable")   != std::string::npos ||
        out.find("not recognized") != std::string::npos ||
        out.find("command not found") != std::string::npos) {
        throw std::runtime_error("[CRITICAL FATAL] Python not found. Engine MUST crash cleanly.");
    }

    std::cout << "   [PythonBridge] PySINDy Worker Output: '" << out << "'" << std::endl;
    std::cout << "   [PythonBridge] Terminating Python Subprocess and returning control to C++ Master." << std::endl;

    // Parse JSON
    std::string eq;
    try {
        nlohmann::json resp = nlohmann::json::parse(out);
        if (resp.value("status","") == "error")
            throw std::runtime_error("[FATAL CRASH] PySINDy worker returned error: " + resp.value("error","unknown"));
        eq = resp.value("discovered_equation", "");
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error(std::string("[FATAL CRASH] PySINDy output invalid JSON: ") + e.what());
    }

    if (eq.empty())
        throw std::runtime_error("[FATAL CRASH] PySINDy failed to generate an equation from input tensors.");

    // Convert equation string → C++ AST
    return parse_math_token(eq);
}

std::string PythonBridge::execute_worker_deepxde(const std::string& script_path, double gamma_noise) {
    std::cout << "   [PythonBridge] Subcontracting PDE resolution to Python DeepXDE Worker..." << std::endl;
    return "weights_optimized.h5"; // still mocked — DeepXDE integration is Phase 7
}

} // namespace q_engine
