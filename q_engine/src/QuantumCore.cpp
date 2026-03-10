#include "QuantumCore.h"
#include <iostream>

namespace q_engine {

QuantumSystem::QuantumSystem(int dim) : dim_(dim), H_(Matrix::Zero(dim, dim)) {}

void QuantumSystem::setHamiltonian(const Hamiltonian& H) {
    if (H.data.rows() != dim_ || H.data.cols() != dim_) {
        throw std::invalid_argument("Hamiltonian dimensions must match system dimensions.");
    }
    H_ = H;
}

void QuantumSystem::addKrausOperator(const KrausOperator& K) {
    if (K.data.rows() != dim_ || K.data.cols() != dim_) {
        throw std::invalid_argument("Kraus operator dimensions must match system dimensions.");
    }
    kraus_operators_.push_back(K);
}

DensityMatrix QuantumSystem::applyKrausMap(const DensityMatrix& rho) const {
    Matrix new_rho = Matrix::Zero(dim_, dim_);
    for (const auto& K : kraus_operators_) {
        new_rho += K.data * rho.data * K.data.adjoint();
    }
    return DensityMatrix(new_rho);
}

Matrix QuantumSystem::lindbladian(const Matrix& rho) const {
    Complex i(0.0, 1.0);
    
    // Commutator: -i[H, rho]
    Matrix commutator = -i * (H_.data * rho - rho * H_.data);
    
    // Dissipator
    Matrix dissipator = Matrix::Zero(dim_, dim_);
    for (const auto& K : kraus_operators_) {
        Matrix k_dagger_k = K.data.adjoint() * K.data;
        // K * rho * K^dagger - 0.5 * (K^dagger * K * rho + rho * K^dagger * K)
        dissipator += K.data * rho * K.data.adjoint() - 0.5 * (k_dagger_k * rho + rho * k_dagger_k);
    }
    
    return commutator + dissipator;
}

DensityMatrix QuantumSystem::evolveLindbladRK4(const DensityMatrix& rho, double dt) const {
    Matrix k1 = lindbladian(rho.data);
    Matrix k2 = lindbladian(rho.data + 0.5 * dt * k1);
    Matrix k3 = lindbladian(rho.data + 0.5 * dt * k2);
    Matrix k4 = lindbladian(rho.data + dt * k3);
    
    Matrix new_rho = rho.data + (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
    return DensityMatrix(new_rho);
}

} // namespace q_engine
