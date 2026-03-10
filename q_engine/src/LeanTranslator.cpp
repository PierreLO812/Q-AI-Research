#include "LeanTranslator.h"

namespace q_engine {

std::string LeanTranslator::translate_expression(ExprPtr expr) {
    if (!expr) return "";

    switch (expr->type) {
        case NodeType::CONSTANT: return std::to_string(expr->value);
        case NodeType::VARIABLE: return expr->name;
        case NodeType::ADD: return "(" + translate_expression(expr->left) + " + " + translate_expression(expr->right) + ")";
        case NodeType::SUB: return "(" + translate_expression(expr->left) + " - " + translate_expression(expr->right) + ")";
        case NodeType::MUL: return "(" + translate_expression(expr->left) + " * " + translate_expression(expr->right) + ")";
        case NodeType::DIV: return "(" + translate_expression(expr->left) + " / " + translate_expression(expr->right) + ")";
        case NodeType::SQUARE: return "(" + translate_expression(expr->left) + "^2)";
    }
    return "";
}

std::string LeanTranslator::generate_theorem_file(const std::string& theorem_name, ExprPtr lhs, ExprPtr rhs, const std::string& tactic) {
    std::string lean_code = "import Mathlib\n\n";
    lean_code += "theorem " + theorem_name + " (x t : ℝ) : ";
    lean_code += translate_expression(lhs) + " = " + translate_expression(rhs) + " := by\n";
    lean_code += "  " + tactic + "\n";
    return lean_code;
}

} // namespace q_engine
