#include "QuantumCore.h"
#include <iostream>
#include <cmath>

using namespace q_engine;

int main() {
    std::cout << "Running QuantumCore tests..." << std::endl;

    // 1. One qubit system (dim = 2)
    QuantumSystem system(2);

    // Initial state: |0><0|
    Matrix rho0 = Matrix::Zero(2, 2);
    rho0(0, 0) = 1.0;
    DensityMatrix rho(rho0);

    if (!rho.isValid()) {
        std::cerr << "Test failed: Initial trace is not 1." << std::endl;
        return 1;
    }

    // Hamiltonian: Pauli X 
    Matrix sigmax = Matrix::Zero(2, 2);
    sigmax(0, 1) = 1.0;
    sigmax(1, 0) = 1.0;
    system.setHamiltonian(Hamiltonian(sigmax));

    // Amplitude damping channel (Kraus operators)
    double gamma = 0.1; // decay rate
    Matrix K0 = Matrix::Zero(2, 2);
    K0(0,0) = 1.0;
    K0(1,1) = std::sqrt(1.0 - gamma);
    
    Matrix K1 = Matrix::Zero(2, 2);
    K1(0,1) = std::sqrt(gamma);

    system.addKrausOperator(KrausOperator(K0));
    system.addKrausOperator(KrausOperator(K1));

    // Test Kraus map Trace Preservation
    DensityMatrix rho_kraus = system.applyKrausMap(rho);

    if (!rho_kraus.isValid()) {
         std::cerr << "Test failed: Kraus map did not preserve trace. Trace is " << rho_kraus.trace() << std::endl;
         return 1;
    }

    // Evolve using Lindblad Master Equation solver
    DensityMatrix rho_t = rho;
    double dt = 0.01;
    for (int step = 0; step < 100; ++step) {
        rho_t = system.evolveLindbladRK4(rho_t, dt);
    }

    if (!rho_t.isValid()) {
         std::cerr << "Test failed: Lindblad RK4 did not preserve trace. Trace is " << rho_t.trace() << std::endl;
         return 1;
    }

    std::cout << "Final Density Matrix after Lindblad evolution:" << std::endl;
    std::cout << rho_t.data << std::endl;

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
