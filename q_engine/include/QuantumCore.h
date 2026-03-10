#ifndef QUANTUM_CORE_H
#define QUANTUM_CORE_H

#include <Eigen/Dense>
#include <vector>
#include <complex>

namespace q_engine {

using Complex = std::complex<double>;
using Matrix = Eigen::MatrixXcd;

// Structure to hold a density matrix rho
struct DensityMatrix {
    Matrix data;

    DensityMatrix(int dim) {
        data = Matrix::Zero(dim, dim);
    }
    
    DensityMatrix(const Matrix& m) : data(m) {}

    double trace() const {
        return data.trace().real();
    }

    bool isValid() const {
        return std::abs(trace() - 1.0) < 1e-9;
    }
};

// Represents a Kraus operator K_i
struct KrausOperator {
    Matrix data;
    
    KrausOperator(const Matrix& m) : data(m) {}
};

// Represents the system Hamiltonian H
struct Hamiltonian {
    Matrix data;

    Hamiltonian(const Matrix& m) : data(m) {}
};

class QuantumSystem {
public:
    QuantumSystem(int dim);

    // Set the Hamiltonian of the system
    void setHamiltonian(const Hamiltonian& H);

    // Add a Kraus operator representing an environment interaction channel
    void addKrausOperator(const KrausOperator& K);

    // Apply Kraus map: rho => sum_i K_i * rho * K_i^dagger
    DensityMatrix applyKrausMap(const DensityMatrix& rho) const;

    // Evolve the density matrix using Lindblad Master Equation over time dt
    // using Runge-Kutta 4th Order.
    DensityMatrix evolveLindbladRK4(const DensityMatrix& rho, double dt) const;

private:
    int dim_;
    Hamiltonian H_;
    std::vector<KrausOperator> kraus_operators_;

    // Computes the Lindbladian superoperator L(rho)
    // L(rho) = -i[H, rho] + sum_k (K_k * rho * K_k^dagger - 0.5 * {K_k^dagger * K_k, rho})
    Matrix lindbladian(const Matrix& rho) const;
};

} // namespace q_engine

#endif // QUANTUM_CORE_H
