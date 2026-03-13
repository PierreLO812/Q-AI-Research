#include "AlHilbert.h"
#include "Positivstellensatz.h"
#include "DimensionalAnalysis.h"
#include <random>
#include <iostream>

namespace q_engine {

static std::mt19937 gen(42);
static std::uniform_real_distribution<> val_dist(-5.0, 5.0);
// Phase 9: Expanded operator distribution (now includes quantum operators)
static std::uniform_int_distribution<> op_dist(0, 9);  // 10 operators

ExprPtr AlHilbertCore::random_expr(int depth) {
    if (depth == 0) {
        // Terminals: constants, physics variables, quantum state
        int choice = op_dist(gen) % 5;
        if (choice == 0) return make_const(val_dist(gen));
        if (choice == 1) return make_var("gamma", "dimensionless");
        if (choice == 2) return make_var("t", "s");
        if (choice == 3) return make_density_matrix("rho");
        // Pauli matrix terminals
        if (choice == 4) {
            int axis = op_dist(gen) % 3;
            if (axis == 0) return make_pauli(PauliAxis::X);
            if (axis == 1) return make_pauli(PauliAxis::Y);
            return make_pauli(PauliAxis::Z);
        }
        return make_var("x");
    }

    int op = op_dist(gen);

    // ─── Classical arithmetic ─────────────────────────
    if (op == 0) {
        // ADD: safe for scalars, must not mix with ρ — catch the type error
        try { return make_add(random_expr(depth - 1), random_expr(depth - 1)); }
        catch (const std::exception&) { return make_exp(make_mul(make_const(-1.0), make_var("gamma"))); }
    }
    if (op == 1) {
        // SUB: type-enforced; if rho is subtracted from scalar, it throws
        try { return make_sub(random_expr(depth - 1), random_expr(depth - 1)); }
        catch (const std::exception&) { return make_var("gamma"); } // fallback to scalar
    }
    if (op == 2) return make_mul(random_expr(depth - 1), random_expr(depth - 1));
    if (op == 3) return make_square(random_expr(depth - 1));

    // ─── Transcendental (noise, dephasing) ────────────
    if (op == 4) return make_tanh(random_expr(depth - 1));         // thermal noise
    if (op == 5) return make_exp(make_mul(                          // e^(-γt)
        make_const(-1.0),
        make_mul(make_var("gamma", "dimensionless"), make_var("t", "s"))));

    // ─── Quantum Algebra (with physical prior bias) ───
    if (op == 6) return make_commutator(random_expr(depth - 1), random_expr(depth - 1));
    if (op == 7) return make_tensor(random_expr(depth - 1), random_expr(depth - 1));
    if (op == 8) {
        // Lindblad dissipator: prior bias for open quantum systems
        double gamma_rate = std::abs(val_dist(gen));
        ExprPtr L = make_pauli(PauliAxis::Z);  // σ_z is most common Lindblad jump op
        ExprPtr rho = make_density_matrix("rho");
        return make_lindblad_dissipator(L, rho, gamma_rate);
    }
    // Dagger / hermitian conjugate
    return make_dagger(random_expr(depth - 1));
}

std::vector<ExprPtr> AlHilbertCore::generate_population(int size, int max_depth) {
    std::vector<ExprPtr> pop;
    for (int i = 0; i < size; ++i) {
        pop.push_back(random_expr(max_depth));
    }
    // PG-SR: Inject known quantum physics templates as strong priors
    // e^(-gamma * t) — exponential decay — most fundamental law of decoherence
    pop[0] = make_exp(make_mul(make_const(-1.0), make_mul(make_var("gamma"), make_var("t", "s"))));
    // tanh(gamma*t) — saturating noise in coupled two-level system
    pop[1] = make_tanh(make_mul(make_var("gamma"), make_var("t", "s")));
    // Lindblad D[σ_z](ρ) — prototypical dephasing channel
    pop[2] = make_lindblad_dissipator(make_pauli(PauliAxis::Z), make_density_matrix("rho"), 0.3);
    return pop;
}

double AlHilbertCore::fitness(ExprPtr expr,
                               const std::vector<std::map<std::string, double>>& data,
                               const std::vector<double>& targets) {
    double error = 0.0;
    for (size_t i = 0; i < data.size(); ++i) {
        try {
            double pred = expr->evaluate(data[i]);
            double err = pred - targets[i];
            error += err * err;
        } catch (...) {
            return 1e9;
        }
    }
    return error / data.size();
}

ExprPtr AlHilbertCore::discover_law(const std::vector<std::map<std::string, double>>& data,
                                     const std::vector<double>& targets,
                                     int generations) {
    auto population = generate_population(100, 3);

    // Build a dimensional map for PG-SR filter
    // Variables: gamma (dimensionless), t (s), rho (dimensionless)
    std::map<std::string, std::string> dim_map = {
        {"gamma", "dimensionless"}, {"t", "s"}, {"x", "dimensionless"},
        {"rho", "dimensionless"}, {"F", "dimensionless"}
    };

    ExprPtr best_law = nullptr;
    double  best_fitness = 1e10;

    for (int g = 0; g < generations; ++g) {
        for (auto& expr : population) {
            // PG-SR: Dimensional Analysis pre-filter (no Lean 4 overhead)
            if (!DimensionalAnalysis::is_dimensionally_valid(expr, dim_map)) {
                continue; // Eliminated before even computing RMSE
            }

            // Positivity check
            if (!CertificateSOS::verify(expr)) continue;

            double f = fitness(expr, data, targets);
            if (f < best_fitness) {
                best_fitness = f;
                best_law = expr;
            }
        }
    }

    if (best_law) {
        std::cout << "   [Al-Hilbert] Best law discovered: " << best_law->to_string()
                  << " (fitness=" << best_fitness << ")" << std::endl;
    }
    return best_law;
}

} // namespace q_engine
