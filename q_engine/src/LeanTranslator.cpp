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
        case NodeType::DAGGER: return "(" + translate_expression(expr->left) + ")†";
        case NodeType::COMMUTATOR: return "[" + translate_expression(expr->left) + ", " + translate_expression(expr->right) + "]";
        case NodeType::ANTICOMMUTATOR: return "{" + translate_expression(expr->left) + ", " + translate_expression(expr->right) + "}";
        case NodeType::TENSOR_PRODUCT: return "(" + translate_expression(expr->left) + " ⊗ " + translate_expression(expr->right) + ")";
        case NodeType::PARTIAL_TRACE: return "Tr_" + std::to_string(expr->trace_subsystem) + "(" + translate_expression(expr->left) + ")";
        case NodeType::LINDBLAD_DISSIPATOR: return "D_(" + std::to_string(expr->gamma) + ")[" + translate_expression(expr->left) + "](" + translate_expression(expr->right) + ")";
        case NodeType::KRAUS_CHANNEL: return "E[" + translate_expression(expr->left) + "](" + translate_expression(expr->right) + ")";
        case NodeType::DENSITY_MATRIX: return expr->name.empty() ? "rho" : expr->name;
    }
    return "";
}

std::string LeanTranslator::generate_theorem_file(const std::string& theorem_name, ExprPtr lhs, ExprPtr rhs, const std::string& tactic) {
    std::string lean_code = "import Mathlib\n\n";
    // Exigence Logique: Câbler statiquement les entités physiques de la fibre, interdire 'x'
    lean_code += "theorem " + theorem_name + " (gamma t : ℝ) (rho : Matrix) : ";
    lean_code += translate_expression(lhs) + " = " + translate_expression(rhs) + " := by\n";
    lean_code += "  " + tactic + "\n";
    return lean_code;
}

} // namespace q_engine
