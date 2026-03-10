#include "Symbolic.h"
#include <stdexcept>

namespace q_engine {

double ExprNode::evaluate(const std::map<std::string, double>& variables) const {
    switch (type) {
        case NodeType::CONSTANT: return value;
        case NodeType::VARIABLE: {
            auto it = variables.find(name);
            if (it != variables.end()) return it->second;
            throw std::runtime_error("Variable not found: " + name);
        }
        case NodeType::ADD: return left->evaluate(variables) + right->evaluate(variables);
        case NodeType::SUB: return left->evaluate(variables) - right->evaluate(variables);
        case NodeType::MUL: return left->evaluate(variables) * right->evaluate(variables);
        case NodeType::DIV: return left->evaluate(variables) / right->evaluate(variables);
        case NodeType::SQUARE: {
            double v = left->evaluate(variables);
            return v * v;
        }
    }
    return 0.0;
}

std::string ExprNode::to_string() const {
    switch (type) {
        case NodeType::CONSTANT: return std::to_string(value);
        case NodeType::VARIABLE: return name;
        case NodeType::ADD: return "(" + left->to_string() + " + " + right->to_string() + ")";
        case NodeType::SUB: return "(" + left->to_string() + " - " + right->to_string() + ")";
        case NodeType::MUL: return "(" + left->to_string() + " * " + right->to_string() + ")";
        case NodeType::DIV: return "(" + left->to_string() + " / " + right->to_string() + ")";
        case NodeType::SQUARE: return "(" + left->to_string() + "^2)";
    }
    return "";
}

ExprPtr ExprNode::clone() const {
    auto c = std::make_shared<ExprNode>(type, value, name);
    if (left) c->left = left->clone();
    if (right) c->right = right->clone();
    return c;
}

ExprPtr make_const(double v) { return std::make_shared<ExprNode>(NodeType::CONSTANT, v); }
ExprPtr make_var(const std::string& n) { return std::make_shared<ExprNode>(NodeType::VARIABLE, 0.0, n); }
ExprPtr make_add(ExprPtr l, ExprPtr r) { 
    auto n = std::make_shared<ExprNode>(NodeType::ADD); n->left = l; n->right = r; return n; 
}
ExprPtr make_sub(ExprPtr l, ExprPtr r) { 
    auto n = std::make_shared<ExprNode>(NodeType::SUB); n->left = l; n->right = r; return n; 
}
ExprPtr make_mul(ExprPtr l, ExprPtr r) { 
    auto n = std::make_shared<ExprNode>(NodeType::MUL); n->left = l; n->right = r; return n; 
}
ExprPtr make_square(ExprPtr l) { 
    auto n = std::make_shared<ExprNode>(NodeType::SQUARE); n->left = l; return n; 
}

} // namespace q_engine
