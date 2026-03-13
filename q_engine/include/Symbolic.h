#ifndef SYMBOLIC_H
#define SYMBOLIC_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace q_engine {

// ═══════════════════════════════════════════════════════════════
// QUANTUM-ENRICHED AST — Phase 9 Action 2
// All physical operators for genuine quantum law discovery
// Reference: Pauli, Lindblad, Kraus, Tensor, Commutator, tanh...
// ═══════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════
// Fix 1 — TYPED AST for ρ
// QuantumType enforces that density matrices are Hermitian, trace-1.
// The type is attached to every ExprNode so the verifier can reject
// any expression that mixes incompatible types (e.g. scalar - rho).
// ═══════════════════════════════════════════════════════════════
enum class QuantumType {
    SCALAR,          // Regular real/complex number or dimensionless quantity
    HERMITIAN_UNIT,  // Hermitian unitary operator (Pauli, etc.)
    DENSITY_OP,      // ρ: Hermitian, PSD, trace-1 — cannot mix with scalar via +/-
    SUPEROPERATOR,   // Lindblad/Kraus superoperator acting on DENSITY_OP
    AMBIGUOUS        // Unknown / not yet resolved
};

// ─── Fix 3 — Physical Constant Annotation ────────────────────────────────
// Every CONSTANT node can carry a physical interpretation so that 4.361548
// is not a naked float but "4.361548 Omega (birefringence rate at 2G)".
struct PhysicalConstant {
    double value = 0.0;
    std::string unit;         // e.g. "Omega", "1/m", "1/s", "dimensionless"
    std::string description;  // e.g. "G-force birefringence coupling"
};

enum class PauliAxis { X, Y, Z };

enum class NodeType {
    // ─── Arithmetic Primitives ───────────────────────────────
    CONSTANT,
    VARIABLE,
    ADD,
    SUB,
    MUL,
    DIV,
    SQUARE,

    // ─── Transcendental (for noise modeling) ─────────────────
    TANH,            // tanh(x)  — thermal fluctuations
    COMPLEX_EXP,     // e^(ix)   — quantum dephasing
    EXP,             // e^(-γt)  — decay envelope

    // ─── Quantum Algebra ─────────────────────────────────────
    PAULI,           // σ_x, σ_y, σ_z
    TENSOR_PRODUCT,  // A ⊗ B
    COMMUTATOR,      // [A, B] = AB - BA
    ANTICOMMUTATOR,  // {A, B} = AB + BA
    DAGGER,          // A†
    PARTIAL_TRACE,   // Tr_A(ρ_AB)

    // ─── Open Quantum Systems (Lindblad/Kraus)  ─────────────
    LINDBLAD_DISSIPATOR, // 𝒟(ρ) = Σ γ_k (L_k ρ L†_k - ½{L†_k L_k, ρ})
    KRAUS_CHANNEL,       // ε(ρ) = Σ K_i ρ K†_i

    // ─── State ───────────────────────────────────────────────
    DENSITY_MATRIX   // ρ
};

class ExprNode;
using ExprPtr = std::shared_ptr<ExprNode>;

class ExprNode {
public:
    NodeType type;
    double value;          // For CONSTANT, or dissipation rate γ
    std::string name;      // For VARIABLE, DENSITY_MATRIX label
    ExprPtr left;
    ExprPtr right;
    double gamma;          // For LINDBLAD_DISSIPATOR
    PauliAxis pauli_axis;  // For PAULI
    std::string dimension; // PG-SR dimensional tag
    int trace_subsystem;   // For PARTIAL_TRACE

    // Fix 1: Quantum Type — enforces that ρ is DENSITY_OP (Hermitian, trace-1)
    QuantumType quantum_type = QuantumType::SCALAR;

    // Fix 3: Physical constant annotation
    PhysicalConstant phys_const; // Populated when type == CONSTANT

    ExprNode(NodeType t, double v = 0.0, const std::string& n = "")
        : type(t), value(v), name(n), gamma(0.0),
          pauli_axis(PauliAxis::Z), dimension("dimensionless"), trace_subsystem(0) {}

    double evaluate(const std::map<std::string, double>& variables) const;
    std::string to_string() const;
    ExprPtr clone() const;
};

// Fix 1: Type Checking — throws std::runtime_error on invalid quantum type mixtures
void assert_type_compatible_binary(const ExprPtr& l, const ExprPtr& r, const std::string& op_name);
QuantumType infer_quantum_type(const ExprPtr& node);

// Fix 3: physical constant factory — annotated with unit and physical description
ExprPtr make_physical_const(double v, const std::string& unit, const std::string& desc);

// ─── Arithmetic ──────────────────────────────────────────────────────────
ExprPtr make_const(double v);
ExprPtr make_var(const std::string& n, const std::string& dim = "dimensionless");
ExprPtr make_add(ExprPtr l, ExprPtr r);
ExprPtr make_sub(ExprPtr l, ExprPtr r);
ExprPtr make_mul(ExprPtr l, ExprPtr r);
ExprPtr make_square(ExprPtr l);

// ─── Transcendental ──────────────────────────────────────────────────────
ExprPtr make_tanh(ExprPtr x);
ExprPtr make_complex_exp(ExprPtr x);
ExprPtr make_exp(ExprPtr x);

// ─── Quantum Algebra ─────────────────────────────────────────────────────
ExprPtr make_pauli(PauliAxis axis);
ExprPtr make_tensor(ExprPtr a, ExprPtr b);
ExprPtr make_commutator(ExprPtr a, ExprPtr b);
ExprPtr make_anticommutator(ExprPtr a, ExprPtr b);
ExprPtr make_dagger(ExprPtr a);
ExprPtr make_partial_trace(ExprPtr rho_ab, int subsystem);

// ─── Open Quantum Systems ────────────────────────────────────────────────
ExprPtr make_lindblad_dissipator(ExprPtr L, ExprPtr rho, double gamma_rate);
ExprPtr make_kraus_channel(ExprPtr K, ExprPtr rho);

// ─── State ───────────────────────────────────────────────────────────────
ExprPtr make_density_matrix(const std::string& label = "rho");

} // namespace q_engine

#endif
