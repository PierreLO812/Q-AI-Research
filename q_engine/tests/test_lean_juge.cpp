#include "LeanTranslator.h"
#include "RLVR_Agent.h"
#include <iostream>

using namespace q_engine;

int main() {
    std::cout << "Testing 'Le Juge' - Formal Prover and RLVR..." << std::endl;

    // Build the AST equation: (x + x) = (x * 2) 
    // Here we model x * 2 as 2 * x for simplicity:
    ExprPtr x = make_var("x");
    ExprPtr lhs = make_add(x, x);                 // x + x
    ExprPtr rhs = make_mul(make_const(2.0), x);   // 2 * x

    // 1. Translation Step
    std::cout << "\n[1] AST Translation:" << std::endl;
    std::string lean_code = LeanTranslator::generate_theorem_file("commutative_identity", lhs, rhs, "sorry");
    std::cout << "Generated Lean 4 Source:\n---\n" << lean_code << "---\n" << std::endl;

    // 2. Proof Step via RLVR (Reinforcement Learning from Verifier Feedback)
    RLVRAgent agent;
    bool proven_safely = agent.prove("commutative_identity", lhs, rhs);
    
    if (proven_safely) {
        std::cout << "\n[SUCCESS] Theorem was absolutely formally verified by the Juge." << std::endl;
    } else {
        std::cerr << "\n[ERROR] The AI could not find a valid formal proof." << std::endl;
        return 1;
    }

    // 3. Negative test (trying to prove something false: x^2 + 2 = x^2 - 1)
    std::cout << "\n[Testing False Theorem Rejection]" << std::endl;
    ExprPtr false_lhs = make_add(make_square(x), make_const(2.0));
    ExprPtr false_rhs = make_sub(make_square(x), make_const(1.0));
    
    bool false_proof_attempt = agent.prove("false_identity", false_lhs, false_rhs);
    if (!false_proof_attempt) {
         std::cout << "[SUCCESS] The false theorem was correctly rejected by Le Juge." << std::endl;
    } else {
         std::cerr << "[ERROR] A false theorem was passed!" << std::endl;
         return 1;
    }

    std::cout << "\nAll RLVR and Lean4 Integration Tests Passed!" << std::endl;
    return 0;
}
