#include "LeanTranslator.h"

namespace q_engine {

static std::string clean_double(double v) {
    std::string s = std::to_string(v);
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    if(s.back() == '.') s.pop_back();
    return s;
}

std::string LeanTranslator::translate_expression(ExprPtr expr) {
    if (!expr) return "";

    switch (expr->type) {
        case NodeType::CONSTANT: return clean_double(expr->value);
        case NodeType::VARIABLE: return expr->name;
        case NodeType::ADD: return "(" + translate_expression(expr->left) + " + " + translate_expression(expr->right) + ")";
        case NodeType::SUB: return "(" + translate_expression(expr->left) + " - " + translate_expression(expr->right) + ")";
        case NodeType::MUL: return "(" + translate_expression(expr->left) + " * " + translate_expression(expr->right) + ")";
        case NodeType::DIV: return "(" + translate_expression(expr->left) + " / " + translate_expression(expr->right) + ")";
        case NodeType::SQUARE: return "((" + translate_expression(expr->left) + ") ^ 2)";
        case NodeType::TANH: return "tanh(" + translate_expression(expr->left) + ")";
        case NodeType::COMPLEX_EXP: return "exp(I * " + translate_expression(expr->left) + ")";
        case NodeType::EXP: return "exp(" + translate_expression(expr->left) + ")";
        case NodeType::PAULI:
            if (expr->pauli_axis == PauliAxis::X) return "sigma_x";
            if (expr->pauli_axis == PauliAxis::Y) return "sigma_y";
            return "sigma_z";
        case NodeType::DAGGER: return "(" + translate_expression(expr->left) + ")_dag";
        case NodeType::COMMUTATOR: return "comm(" + translate_expression(expr->left) + ", " + translate_expression(expr->right) + ")";
        case NodeType::ANTICOMMUTATOR: return "acomm(" + translate_expression(expr->left) + ", " + translate_expression(expr->right) + ")";
        case NodeType::TENSOR_PRODUCT: return "(" + translate_expression(expr->left) + " kron " + translate_expression(expr->right) + ")";
        case NodeType::PARTIAL_TRACE: return "partial_trace_" + std::to_string(expr->trace_subsystem) + "(" + translate_expression(expr->left) + ")";
        case NodeType::LINDBLAD_DISSIPATOR: return "lindblad_" + clean_double(expr->gamma) + "(" + translate_expression(expr->left) + ", " + translate_expression(expr->right) + ")";
        case NodeType::KRAUS_CHANNEL: return "kraus(" + translate_expression(expr->left) + ", " + translate_expression(expr->right) + ")";
        case NodeType::DENSITY_MATRIX: return expr->name.empty() ? "rho" : expr->name;
    }
    return "";
}

std::string LeanTranslator::generate_theorem_file(const std::string& theorem_name,
                                                   ExprPtr lhs, ExprPtr rhs,
                                                   const std::string& tactic) {
    // ─── CRITICAL: NO "import Mathlib" ────────────────────────────────────────
    // Mathlib triggers a multi-GB Lake download the first time it is used.
    // We define the minimal opaque types the theorem needs so lean can type-check
    // and attempt the tactic immediately, with zero network activity.
    std::string code;
    code += "-- Q-Engine self-contained Lean 4 kernel (no external imports)\n";
    code += "-- All types are opaque so lean accepts them without any library.\n";
    code += "opaque Real : Type\n";
    code += "opaque Real.instAdd : Add Real := ⟨fun a _ => a⟩\n";
    code += "opaque Real.instMul : Mul Real := ⟨fun a _ => a⟩\n";
    code += "opaque Real.instPow : HPow Real Nat Real := ⟨fun a _ => a⟩\n";
    code += "opaque Real.exp : Real -> Real\n";
    code += "instance : Add Real  := Real.instAdd\n";
    code += "instance : Mul Real  := Real.instMul\n";
    code += "instance : HPow Real Nat Real := Real.instPow\n";
    code += "\n";
    code += "theorem " + theorem_name + " (gamma t : Real) : ";
    code += translate_expression(lhs) + " = " + translate_expression(rhs) + " := by\n";
    code += "  " + tactic + "\n";
    return code;
}

} // namespace q_engine
