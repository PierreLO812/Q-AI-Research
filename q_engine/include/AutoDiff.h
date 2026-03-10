#ifndef AUTODIFF_H
#define AUTODIFF_H

#include <vector>
#include <memory>
#include <cmath>

namespace q_engine {

class Value;
using ValuePtr = std::shared_ptr<Value>;

// Represents a node in the computation graph for Automatic Differentiation
class Value : public std::enable_shared_from_this<Value> {
public:
    double data;
    double grad;
    std::vector<ValuePtr> children;
    int op;

    Value(double data, std::vector<ValuePtr> children = {}, int op = 0) 
        : data(data), grad(0.0), children(children), op(op) {}

    // Backpropagation to compute derivatives dynamically
    void backward();

private:
    void build_topo(std::vector<ValuePtr>& topo, std::vector<Value*>& visited);
};

// Math Operations that add nodes to the Graph
ValuePtr make_value(double val);
ValuePtr add(ValuePtr a, ValuePtr b);
ValuePtr mul(ValuePtr a, ValuePtr b);
ValuePtr tanh(ValuePtr a);
ValuePtr pow2(ValuePtr a);

// Operator Overload for cleanliness
ValuePtr operator+(ValuePtr a, ValuePtr b);
ValuePtr operator*(ValuePtr a, ValuePtr b);
ValuePtr operator-(ValuePtr a, ValuePtr b);
ValuePtr operator-(ValuePtr a);

} // namespace q_engine

#endif
