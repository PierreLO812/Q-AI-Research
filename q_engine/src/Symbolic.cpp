#include "Symbolic.h"
#include <stdexcept>
#include <cmath>

namespace q_engine {

// ═══════════════════════════════════════════════════════════════════
// ExprNode::evaluate — scalar projection for simplified expressions
// Full matricial evaluation requires the QuantumCore integration.
// ═══════════════════════════════════════════════════════════════════
double ExprNode::evaluate(const std::map<std::string, double>& variables) const {
    switch (type) {
        case NodeType::CONSTANT: return value;
        case NodeType::VARIABLE: {
            auto it = variables.find(name);
            if (it != variables.end()) return it->second;
            throw std::runtime_error("Variable not found: " + name);
        }
        case NodeType::ADD:   return left->evaluate(variables) + right->evaluate(variables);
        case NodeType::SUB:   return left->evaluate(variables) - right->evaluate(variables);
        case NodeType::MUL:   return left->evaluate(variables) * right->evaluate(variables);
        case NodeType::DIV:   return left->evaluate(variables) / right->evaluate(variables);
        case NodeType::SQUARE: {
            double v = left->evaluate(variables);
            return v * v;
        }
        // --- Transcendental ---
        case NodeType::TANH:        return std::tanh(left->evaluate(variables));
        case NodeType::COMPLEX_EXP: return std::cos(left->evaluate(variables)); // Re(e^ix) = cos(x)
        case NodeType::EXP:         return std::exp(left->evaluate(variables));

        // --- Quantum Algebra: scalar projections ---
        // For actual matrix form: pass to QuantumCore
        case NodeType::PAULI:
            // Scalar trace: Tr(σ) = 0 for all Pauli matrices
            return 0.0;
        case NodeType::COMMUTATOR:
            // [A,B] scalar projection (must be compared against simulation data)
            return left->evaluate(variables) * right->evaluate(variables) 
                 - right->evaluate(variables) * left->evaluate(variables);
        case NodeType::ANTICOMMUTATOR:
            return left->evaluate(variables) * right->evaluate(variables) 
                 + right->evaluate(variables) * left->evaluate(variables);
        case NodeType::DAGGER:
            return left->evaluate(variables); // For real matrices: A† = A^T → trace same
        case NodeType::LINDBLAD_DISSIPATOR:
            // Scalar decay envelope: γ * ρ for simplified evaluation
            return -gamma * left->evaluate(variables);
        case NodeType::KRAUS_CHANNEL:
            // Identity action on scalar: Σ K_i * ρ * K†_i ≈ ρ (for unital channel)
            return left->evaluate(variables) * right->evaluate(variables);
        case NodeType::TENSOR_PRODUCT:
            // Tensor product of scalars = multiplication
            return left->evaluate(variables) * right->evaluate(variables);
        case NodeType::PARTIAL_TRACE:
            return left->evaluate(variables); // Trace over subsystem: returns scalar
        case NodeType::DENSITY_MATRIX:
            return 1.0; // Population: Tr(ρ) = 1

        default: return 0.0;
    }
}

// ═══════════════════════════════════════════════════════════════════
// ExprNode::to_string — Physics-notation string representation
// ═══════════════════════════════════════════════════════════════════
std::string ExprNode::to_string() const {
    switch (type) {
        case NodeType::CONSTANT:
            // Fix 3: Include physical unit in to_string if annotated
            if (!phys_const.unit.empty()) {
                return std::to_string(value) + " [" + phys_const.unit + "] (" + phys_const.description + ")";
            }
            return std::to_string(value);
        case NodeType::VARIABLE:   return name;
        case NodeType::ADD:        return "(" + left->to_string() + " + " + right->to_string() + ")";
        case NodeType::SUB:        return "(" + left->to_string() + " - " + right->to_string() + ")";
        case NodeType::MUL:        return "(" + left->to_string() + " * " + right->to_string() + ")";
        case NodeType::DIV:        return "(" + left->to_string() + " / " + right->to_string() + ")";
        case NodeType::SQUARE:     return "(" + left->to_string() + "^2)";
        case NodeType::TANH:       return "tanh(" + left->to_string() + ")";
        case NodeType::COMPLEX_EXP:return "e^(i*" + left->to_string() + ")";
        case NodeType::EXP:        return "e^(" + left->to_string() + ")";
        case NodeType::PAULI:
            if (pauli_axis == PauliAxis::X) return "sigma_x";
            if (pauli_axis == PauliAxis::Y) return "sigma_y";
            return "sigma_z";
        case NodeType::TENSOR_PRODUCT:   return "(" + left->to_string() + " ⊗ " + right->to_string() + ")";
        case NodeType::COMMUTATOR:       return "[" + left->to_string() + ", " + right->to_string() + "]";
        case NodeType::ANTICOMMUTATOR:   return "{" + left->to_string() + ", " + right->to_string() + "}";
        case NodeType::DAGGER:           return left->to_string() + "†";
        case NodeType::PARTIAL_TRACE:    return "Tr_" + std::to_string(trace_subsystem) + "(" + left->to_string() + ")";
        case NodeType::LINDBLAD_DISSIPATOR:
            return "D_gamma=" + std::to_string(gamma) + "[" + left->to_string() + "](" + right->to_string() + ")";
        case NodeType::KRAUS_CHANNEL:    return "E[" + left->to_string() + "](" + right->to_string() + ")";
        case NodeType::DENSITY_MATRIX:   return name.empty() ? "rho" : name;
        default:                         return "?";
    }
}

ExprPtr ExprNode::clone() const {
    auto c = std::make_shared<ExprNode>(type, value, name);
    c->gamma        = gamma;
    c->pauli_axis   = pauli_axis;
    c->dimension    = dimension;
    c->quantum_type = quantum_type;
    c->phys_const   = phys_const;
    c->trace_subsystem = trace_subsystem;
    if (left)  c->left  = left->clone();
    if (right) c->right = right->clone();
    return c;
}

// ═══════════════════════════════════════════════════════════════
// Fix 1: Quantum Type Inference and Enforcement
// ═══════════════════════════════════════════════════════════════
QuantumType infer_quantum_type(const ExprPtr& node) {
    if (!node) return QuantumType::AMBIGUOUS;
    switch (node->type) {
        case NodeType::DENSITY_MATRIX:       return QuantumType::DENSITY_OP;
        case NodeType::PAULI:                return QuantumType::HERMITIAN_UNIT;
        case NodeType::LINDBLAD_DISSIPATOR:  return QuantumType::SUPEROPERATOR;
        case NodeType::KRAUS_CHANNEL:        return QuantumType::SUPEROPERATOR;
        case NodeType::COMMUTATOR:           return QuantumType::HERMITIAN_UNIT;
        case NodeType::ANTICOMMUTATOR:       return QuantumType::HERMITIAN_UNIT;
        case NodeType::DAGGER:               return infer_quantum_type(node->left);
        case NodeType::TENSOR_PRODUCT:       return QuantumType::HERMITIAN_UNIT;
        case NodeType::PARTIAL_TRACE:        return QuantumType::SCALAR; // Trace → scalar
        case NodeType::CONSTANT:             return QuantumType::SCALAR;
        case NodeType::VARIABLE:             return QuantumType::SCALAR;
        case NodeType::EXP:
        case NodeType::TANH:
        case NodeType::COMPLEX_EXP:          return QuantumType::SCALAR;
        default:                             return QuantumType::AMBIGUOUS;
    }
}

void assert_type_compatible_binary(const ExprPtr& l, const ExprPtr& r, const std::string& op_name) {
    if (!l || !r) return;
    auto tl = infer_quantum_type(l);
    auto tr = infer_quantum_type(r);

    // CRITICAL: Cannot add/subtract a scalar and a density matrix
    bool l_is_density = (tl == QuantumType::DENSITY_OP);
    bool r_is_density = (tr == QuantumType::DENSITY_OP);
    bool l_is_scalar  = (tl == QuantumType::SCALAR);
    bool r_is_scalar  = (tr == QuantumType::SCALAR);

    if ((l_is_density && r_is_scalar) || (l_is_scalar && r_is_density)) {
        throw std::runtime_error(
            "[AST TYPE ERROR] Operation '" + op_name +
            "' is illegal: cannot mix a density matrix rho (Hermitian, trace-1) "
            "with a scalar. The equation is physically meaningless and rejected."
        );
    }
}

// ─── Arithmetic ──────────────────────────────────────────────────────────
ExprPtr make_const(double v) { return std::make_shared<ExprNode>(NodeType::CONSTANT, v); }
ExprPtr make_var(const std::string& n, const std::string& dim) {
    auto e = std::make_shared<ExprNode>(NodeType::VARIABLE, 0.0, n);
    e->dimension = dim;
    return e;
}
// Fix 1: Type-aware binary operations — validate before constructing
ExprPtr make_add(ExprPtr l, ExprPtr r) {
    assert_type_compatible_binary(l, r, "+");
    auto n = std::make_shared<ExprNode>(NodeType::ADD); n->left = l; n->right = r; return n;
}
ExprPtr make_sub(ExprPtr l, ExprPtr r) {
    assert_type_compatible_binary(l, r, "-");
    auto n = std::make_shared<ExprNode>(NodeType::SUB); n->left = l; n->right = r; return n;
}
ExprPtr make_mul(ExprPtr l, ExprPtr r) {
    auto n = std::make_shared<ExprNode>(NodeType::MUL); n->left = l; n->right = r; return n;
}
ExprPtr make_square(ExprPtr l) {
    auto n = std::make_shared<ExprNode>(NodeType::SQUARE); n->left = l; return n;
}

// ─── Transcendental ──────────────────────────────────────────────────────
ExprPtr make_tanh(ExprPtr x)        { auto n = std::make_shared<ExprNode>(NodeType::TANH);        n->left = x; return n; }
ExprPtr make_complex_exp(ExprPtr x) { auto n = std::make_shared<ExprNode>(NodeType::COMPLEX_EXP); n->left = x; return n; }
ExprPtr make_exp(ExprPtr x)         { auto n = std::make_shared<ExprNode>(NodeType::EXP);         n->left = x; return n; }

// ─── Quantum Algebra ─────────────────────────────────────────────────────
ExprPtr make_pauli(PauliAxis axis) {
    auto n = std::make_shared<ExprNode>(NodeType::PAULI);
    n->pauli_axis = axis;
    return n;
}
ExprPtr make_tensor(ExprPtr a, ExprPtr b) {
    auto n = std::make_shared<ExprNode>(NodeType::TENSOR_PRODUCT); n->left = a; n->right = b; return n;
}
ExprPtr make_commutator(ExprPtr a, ExprPtr b) {
    auto n = std::make_shared<ExprNode>(NodeType::COMMUTATOR); n->left = a; n->right = b; return n;
}
ExprPtr make_anticommutator(ExprPtr a, ExprPtr b) {
    auto n = std::make_shared<ExprNode>(NodeType::ANTICOMMUTATOR); n->left = a; n->right = b; return n;
}
ExprPtr make_dagger(ExprPtr a) {
    auto n = std::make_shared<ExprNode>(NodeType::DAGGER); n->left = a; return n;
}
ExprPtr make_partial_trace(ExprPtr rho_ab, int subsystem) {
    auto n = std::make_shared<ExprNode>(NodeType::PARTIAL_TRACE);
    n->left = rho_ab;
    n->trace_subsystem = subsystem;
    return n;
}

// ─── Open Quantum Systems ────────────────────────────────────────────────
ExprPtr make_lindblad_dissipator(ExprPtr L, ExprPtr rho, double gamma_rate) {
    auto n = std::make_shared<ExprNode>(NodeType::LINDBLAD_DISSIPATOR);
    n->left  = L;
    n->right = rho;
    n->gamma = gamma_rate;
    return n;
}
ExprPtr make_kraus_channel(ExprPtr K, ExprPtr rho) {
    auto n = std::make_shared<ExprNode>(NodeType::KRAUS_CHANNEL); n->left = K; n->right = rho; return n;
}

// ─── State ───────────────────────────────────────────────────────────────
ExprPtr make_density_matrix(const std::string& label) {
    return std::make_shared<ExprNode>(NodeType::DENSITY_MATRIX, 0.0, label);
}

// Fix 3: Annotated physical constant factory
ExprPtr make_physical_const(double v, const std::string& unit, const std::string& desc) {
    auto n = std::make_shared<ExprNode>(NodeType::CONSTANT, v);
    n->phys_const = {v, unit, desc};
    return n;
}

} // namespace q_engine
