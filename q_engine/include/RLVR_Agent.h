#ifndef RLVR_AGENT_H
#define RLVR_AGENT_H

#include "Symbolic.h"
#include <string>
#include <vector>
#include <map>

namespace q_engine {

// ═══════════════════════════════════════════════════════════════
// RLVR AGENT — Phase 9 Action 1 (Composite Reward Function)
//
// Reward = Physics_Accuracy(E, simulation_data) × Lean4_Score(E)
//
// An equation that doesn't model real quantum decoherence
// gets Reward=0 even if Lean 4 finds it mathematically valid.
// ═══════════════════════════════════════════════════════════════

class RLVRAgent {
public:
    RLVRAgent() {
        tactics_ = {"aesop", "quantum_simp", "simp", "linarith"};
    }

    // ─── Lean 4 Formal Proof ──────────────────────────────────
    // Returns true if Lean 4 proves the theorem (Lean4_Score = 1)
    bool prove(const std::string& theorem_name, ExprPtr lhs, ExprPtr rhs);

    // ─── Physics Accuracy Score ───────────────────────────────
    // Compares equation prediction against Lindblad simulation data points.
    // Returns a score in [0, 1]: 1.0 = perfect match, 0.0 = no match.
    double compute_physics_accuracy(ExprPtr equation,
                                    const std::vector<double>& reference_data,
                                    const std::vector<std::map<std::string, double>>& variable_sets);

    // ─── Composite Reward ─────────────────────────────────────
    // Reward = physics_accuracy * lean4_score
    // The equation must satisfy BOTH physics AND formal logic.
    double compute_reward(ExprPtr equation,
                          ExprPtr lean_target,
                          const std::string& theorem_name,
                          const std::vector<double>& reference_data,
                          const std::vector<std::map<std::string, double>>& variable_sets);

    std::string get_last_lean_error() const { return last_lean_error_; }

private:
    std::vector<std::string> tactics_;
    std::string last_lean_error_;

    // Root Mean Square Error between equation predictions and reference
    double rmse(const std::vector<double>& predictions,
                const std::vector<double>& reference);
};

} // namespace q_engine

#endif
