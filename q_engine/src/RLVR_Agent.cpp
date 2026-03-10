#include "RLVR_Agent.h"
#include "LeanTranslator.h"
#include "LeanInterface.h"
#include <iostream>

namespace q_engine {

bool RLVRAgent::prove(const std::string& theorem_name, ExprPtr lhs, ExprPtr rhs) {
    std::cout << "\n[RLVR] Generating formal proof for " << theorem_name << "..." << std::endl;
    
    for (const auto& tactic : tactics_) {
        std::string code = LeanTranslator::generate_theorem_file(theorem_name, lhs, rhs, tactic);
        std::cout << "[RLVR] Trying tactic: " << tactic << std::endl;
        
        LeanResult result = LeanInterface::verify_theorem(code);
        
        if (result.success) {
            std::cout << "[RLVR] SUCCESS! Lean kernel verified the theorem using tactic: " << tactic << std::endl;
            return true;
        } else {
            std::cout << "[RLVR] Tactic failed. Lean feedback: " << result.message << std::endl;
            // The magic of RLVR: The agent looks at the error message, adjusts weights,
            // and tries a different tactic sequence on the next attempt.
        }
    }
    
    std::cerr << "[RLVR] Exhausted tactics. Proof failed." << std::endl;
    return false;
}

} // namespace q_engine
