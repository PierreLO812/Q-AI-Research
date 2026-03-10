#include "AutoDiff.h"
#include <algorithm>

namespace q_engine {

void Value::build_topo(std::vector<ValuePtr>& topo, std::vector<Value*>& visited) {
    if (std::find(visited.begin(), visited.end(), this) == visited.end()) {
        visited.push_back(this);
        for (auto& child : children) {
            child->build_topo(topo, visited);
        }
        topo.push_back(shared_from_this());
    }
}

void Value::backward() {
    std::vector<ValuePtr> topo;
    std::vector<Value*> visited;
    build_topo(topo, visited);
    
    // Reverse topological order
    std::reverse(topo.begin(), topo.end());
    
    this->grad = 1.0;
    
    for (auto& v : topo) {
        if (v->op == 1) { // add
            v->children[0]->grad += v->grad;
            v->children[1]->grad += v->grad;
        } else if (v->op == 2) { // mul
            v->children[0]->grad += v->children[1]->data * v->grad;
            v->children[1]->grad += v->children[0]->data * v->grad;
        } else if (v->op == 3) { // tanh
            double dtanh = 1.0 - (v->data * v->data);
            v->children[0]->grad += dtanh * v->grad;
        } else if (v->op == 4) { // pow2
            v->children[0]->grad += 2.0 * v->children[0]->data * v->grad;
        }
    }
}

ValuePtr make_value(double val) {
    return std::make_shared<Value>(val);
}

ValuePtr add(ValuePtr a, ValuePtr b) {
    return std::make_shared<Value>(a->data + b->data, std::vector<ValuePtr>{a, b}, 1);
}

ValuePtr mul(ValuePtr a, ValuePtr b) {
    return std::make_shared<Value>(a->data * b->data, std::vector<ValuePtr>{a, b}, 2);
}

ValuePtr tanh(ValuePtr a) {
    return std::make_shared<Value>(std::tanh(a->data), std::vector<ValuePtr>{a}, 3);
}

ValuePtr pow2(ValuePtr a) {
    return std::make_shared<Value>(a->data * a->data, std::vector<ValuePtr>{a}, 4);
}

ValuePtr operator+(ValuePtr a, ValuePtr b) { return add(a, b); }
ValuePtr operator*(ValuePtr a, ValuePtr b) { return mul(a, b); }
ValuePtr operator-(ValuePtr a, ValuePtr b) { 
    return add(a, mul(make_value(-1.0), b)); 
}
ValuePtr operator-(ValuePtr a) { 
    return mul(make_value(-1.0), a); 
}

} // namespace q_engine
