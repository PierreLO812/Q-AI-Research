#include "AlHilbert.h"
#include "Positivstellensatz.h"
#include <iostream>
#include <vector>
#include <map>

using namespace q_engine;

int main() {
    std::cout << "Testing 'Le Cerveau' - Al-Hilbert Symbolic Engine..." << std::endl;

    AlHilbertCore al_hilbert;

    // 1. Build Mock Data
    // Let's pretend the mysterious physics data follows the rule: y = x^2 + 2
    std::vector<std::map<std::string, double>> data;
    std::vector<double> targets;
    
    for (double i = -5.0; i <= 5.0; i += 1.0) {
        data.push_back({{"x", i}});
        targets.push_back(i * i + 2.0);
    }

    // 2. Discover the Law 
    // The discovery engine should filter out structurally impossible equations
    // and converge on x^2 + 2 through fitness minimization.
    ExprPtr discovered_law = al_hilbert.discover_law(data, targets, 10);

    if (discovered_law) {
        std::cout << "\n[SUCCESS] Al-Hilbert AI Discovered Law: y = " << discovered_law->to_string() << std::endl;
        
        // 3. Verify Law Constraint using Positivstellensatz
        bool is_sos = CertificateSOS::verify(discovered_law);
        if (is_sos) {
            std::cout << "[SUCCESS] CertificateSOS Validated: The law is structurally guaranteed to be >= 0 (Sum of Squares)." << std::endl;
        } else {
            std::cerr << "[ERROR] The law failed the physical constraints check!" << std::endl;
            return 1;
        }

    } else {
        std::cerr << "[ERROR] Al-Hilbert could not find a law satisfying the Positivstellensatz constraints." << std::endl;
        return 1;
    }

    // 4. Test SOS rejection
    ExprPtr dangerous_law = make_sub(make_square(make_var("x")), make_const(2.0)); // x^2 - 2 -> can be negative!
    std::cout << "\nTesting a dangerous law: " << dangerous_law->to_string() << std::endl;
    if (!CertificateSOS::verify(dangerous_law)) {
        std::cout << "[SUCCESS] CertificateSOS correctly rejected the dangerous law because it violates positivity constraints." << std::endl;
    } else {
        std::cerr << "[ERROR] CertificateSOS falsely approved a bad law." << std::endl;
        return 1;
    }

    std::cout << "\nAll Al-Hilbert tests passed." << std::endl;
    return 0;
}
