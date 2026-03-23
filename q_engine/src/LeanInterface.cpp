#include "LeanInterface.h"
#include <iostream>
#include <fstream>
#include <array>
#include <cstdio>

extern "C" {
    FILE* popen(const char* command, const char* type);
    int pclose(FILE* stream);
}

namespace q_engine {

LeanResult LeanInterface::verify_theorem(const std::string& lean_code) {
    // Ticket 3: Authentic Native Execution of Lean 4
    
    // Write theorem to disk
    std::ofstream out("run_theorem.lean");
    out << lean_code;
    out.close();

    // Call physical Lean 4 compiler via popen
    std::string cmd = "lean run_theorem.lean 2>&1";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        return {false, "System Error: C++ popen() failed to invoke Lean 4 compiler."};
    }

    std::array<char, 256> buffer;
    std::string output;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        output += buffer.data();
    }
    int exit_code = pclose(pipe);

    // Completely Native Error Catching!
    // Lean 4 crashes on 'unknown identifier D_' yielding an exit_code != 0
    if (exit_code != 0 || output.find("error") != std::string::npos) {
        return {false, output}; // Directly pass authentic Lean 4 stdout/stderr to LLM
    }

    return {true, "Complete Proof (Native Exit 0)"};
}

} // namespace q_engine
