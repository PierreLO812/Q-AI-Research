#ifndef RLVR_AGENT_H
#define RLVR_AGENT_H

#include "Symbolic.h"
#include <string>
#include <vector>

namespace q_engine {

class RLVRAgent {
public:
    RLVRAgent() {
        // Known Lean 4 Tactics pool
        tactics_ = {"sorry", "rfl", "simp", "ring", "linarith"};
    }

    // Attempt to prove an equation iteratively via Reinforcement Learning over Verifier Feedback
    bool prove(const std::string& theorem_name, ExprPtr lhs, ExprPtr rhs);

private:
    std::vector<std::string> tactics_;
};

} // namespace q_engine

#endif
