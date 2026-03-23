#include "RLVR_Agent.h"
#include "LeanTranslator.h"
#include "LeanInterface.h"
#include <iostream>
#include <cmath>
#include <stdexcept>

namespace q_engine {

// ─── Lean 4 Formal Proof ──────────────────────────────────────────────────
bool RLVRAgent::prove(const std::string& theorem_name, ExprPtr lhs, ExprPtr rhs) {
    std::cout << "\n[RLVR] Generating formal proof for " << theorem_name << "..." << std::endl;

    for (const auto& tactic : tactics_) {
        std::string code = LeanTranslator::generate_theorem_file(theorem_name, lhs, rhs, tactic);
        std::cout << "[RLVR] Generated Lean 4 Code:\n" << code << std::endl;
        std::cout << "[RLVR] Trying tactic: " << tactic << std::endl;

        LeanResult result = LeanInterface::verify_theorem(code);

        if (result.success) {
            std::cout << "[RLVR] SUCCESS! Lean kernel verified theorem using tactic: " << tactic << std::endl;
            last_lean_error_ = "";
            return true;
        } else {
            std::cout << "[RLVR] Tactic failed. Lean feedback: " << result.message << std::endl;
            last_lean_error_ = result.message; // Propagate exact stderr to Mutator
        }
    }

    std::cerr << "[RLVR] Exhausted tactics. Proof failed." << std::endl;
    return false;
}

// ─── Physics Accuracy Score ───────────────────────────────────────────────
double RLVRAgent::rmse(const std::vector<double>& predictions, const std::vector<double>& reference) {
    if (predictions.size() != reference.size() || predictions.empty()) return 1.0; // worst
    double sum = 0.0;
    for (size_t i = 0; i < predictions.size(); ++i) {
        double diff = predictions[i] - reference[i];
        sum += diff * diff;
    }
    return std::sqrt(sum / predictions.size());
}

double RLVRAgent::compute_physics_accuracy(ExprPtr equation,
                                            const std::vector<double>& reference_data,
                                            const std::vector<std::map<std::string, double>>& variable_sets) {
    if (!equation || reference_data.empty() || variable_sets.empty()) return 0.0;

    std::vector<double> predictions;
    for (const auto& vars : variable_sets) {
        try {
            predictions.push_back(equation->evaluate(vars));
        } catch (...) {
            return 0.0; // Invalid evaluation = no accuracy
        }
    }

    double error = rmse(predictions, reference_data);
    // Convert RMSE to [0,1] accuracy; cap at 1.0 for very good fit
    double accuracy = std::max(0.0, 1.0 - error);
    std::cout << "   [RLVR] Physics accuracy score: " << accuracy
              << " (RMSE vs Lindblad simulation: " << error << ")" << std::endl;
    return accuracy;
}

// ─── Composite Reward ─────────────────────────────────────────────────────
// Reward = physics_accuracy × lean4_score
// Both must be satisfied for a non-zero reward.
double RLVRAgent::compute_reward(ExprPtr equation,
                                  ExprPtr lean_target,
                                  const std::string& theorem_name,
                                  const std::vector<double>& reference_data,
                                  const std::vector<std::map<std::string, double>>& variable_sets) {
    std::cout << "\n[RLVR] Computing Composite Reward..." << std::endl;

    // Step 1: Physics accuracy
    double physics_score = compute_physics_accuracy(equation, reference_data, variable_sets);
    if (physics_score < 0.01) {
        std::cout << "   [RLVR] Physics accuracy too low (" << physics_score
                  << "). Skipping Lean 4 call. Reward = 0." << std::endl;
        return 0.0;
    }

    // Step 2: Lean 4 formal proof
    double lean_score = prove(theorem_name, equation, lean_target) ? 1.0 : 0.0;

    // Composite
    double reward = physics_score * lean_score;
    std::cout << "   [RLVR] Final Reward = " << physics_score << " * " << lean_score
              << " = " << reward << std::endl;
    return reward;
}

} // namespace q_engine
