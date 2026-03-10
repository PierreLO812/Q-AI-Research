#ifndef SYMBOLIC_H
#define SYMBOLIC_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace q_engine {

enum class NodeType {
    CONSTANT,
    VARIABLE,
    ADD,
    SUB,
    MUL,
    DIV,
    SQUARE
};

class ExprNode;
using ExprPtr = std::shared_ptr<ExprNode>;

class ExprNode {
public:
    NodeType type;
    double value; // For CONSTANT
    std::string name; // For VARIABLE
    ExprPtr left;
    ExprPtr right;

    ExprNode(NodeType t, double v = 0.0, const std::string& n = "") 
        : type(t), value(v), name(n) {}

    double evaluate(const std::map<std::string, double>& variables) const;
    std::string to_string() const;
    ExprPtr clone() const;
};

// Factory functions
ExprPtr make_const(double v);
ExprPtr make_var(const std::string& n);
ExprPtr make_add(ExprPtr l, ExprPtr r);
ExprPtr make_sub(ExprPtr l, ExprPtr r);
ExprPtr make_mul(ExprPtr l, ExprPtr r);
ExprPtr make_square(ExprPtr l);

} // namespace q_engine

#endif
