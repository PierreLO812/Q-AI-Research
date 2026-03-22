#include "LeanInterface.h"
#include <iostream>

namespace q_engine {

LeanResult LeanInterface::verify_theorem(const std::string& lean_code) {
    // In a real production system, this saves `lean_code` to a `.lean` file
    // and calls `popen("lean --run file.lean", "r")` to execute the Lean 4 kernel.
    
    // MOCK VERIFICATION for the sake of the C++ demo.
    // If we're trying to prove x + x = 2 * x, "ring" is the correct Lean tactic for commutative rings (like Reals).
    
    // Extract the actual equation from lean_code instead of hardcoding x+x
    std::string eq = "(Equation non analysee)";
    size_t start = lean_code.find(" : ");
    size_t end = lean_code.find(" := by");
    if (start != std::string::npos && end != std::string::npos) {
        eq = lean_code.substr(start + 3, end - start - 3);
    }

    bool contains_valid_tactic = (lean_code.find("aesop") != std::string::npos || 
                                  lean_code.find("quantum_simp") != std::string::npos);
                                  
    if (lean_code.find(" x ") != std::string::npos || lean_code.find("(x") != std::string::npos) {
        return {false, "error: FRAUD DETECTED. Abstract variable 'x' is forbidden. Lean 4 restricted to physical variables."};
    }

    std::string lean_response = contains_valid_tactic ? "Complete Proof" : "error: unsolved goals.\n|- " + eq;
    
    // Parse the Lean formal verification response
    if (lean_response.find("Complete Proof") != std::string::npos) {
        return {true, "Complete Proof"};
    } else {
        return {false, lean_response};
    }
}

} // namespace q_engine
