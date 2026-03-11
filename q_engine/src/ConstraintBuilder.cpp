#include "ConstraintBuilder.h"
#include <cmath>
#include <iostream>

namespace q_engine {

ExprPtr ConstraintBuilder::build_fidelity_constraint(const std::vector<PhysicalEntity>& entities) {
    // Ex: "Fidelity > 0.9" is a required constraint if the distance is < 100km, else 0.5
    double target_fidelity = 0.8; // default
    
    for (const auto& entity : entities) {
        if (entity.type == "distance" && entity.value > 50000.0) {
            target_fidelity = 0.5; // lower baseline for long-distance fibers
        }
        if (entity.type == "pressure" && entity.value > 1.5) {
            target_fidelity += 0.1; // Harder constraints for high pressure (req: stronger signal)
        }
    }
    
    // Return expression: F - target_fidelity (which Positivstellensatz will check is > 0)
    return make_add(make_var("F"), make_const(-target_fidelity));
}

double ConstraintBuilder::calculate_gamma_from_entities(const std::vector<PhysicalEntity>& entities) {
    // Base dissipation
    double gamma = 0.01; 
    
    for (const auto& entity : entities) {
        if (entity.type == "distance") {
            // Optical fiber attenuation (approx. 0.2 dB/km -> scaling factor)
            gamma += (entity.value / 1000.0) * 0.05; 
            std::cout << "   -> Physics Engine Adjusted: Fiber Attenuation Noise added for " << entity.value << "m." << std::endl;
        }
        else if (entity.type == "pressure") {
            // Dephasing induced by pressure/vibrations (G)
            gamma += (entity.value * 0.15);
            std::cout << "   -> Physics Engine Adjusted: G-Force Dephasing Noise added for " << entity.value << "G." << std::endl;
        }
        else if (entity.type == "temperature") {
            // Thermal equilibrium shift
            double temp_c = entity.unit == "K" ? entity.value - 273.15 : entity.value;
            gamma += (temp_c > 20.0 ? (temp_c - 20.0) * 0.005 : 0);
            std::cout << "   -> Physics Engine Adjusted: Thermal Noise added for " << entity.value << entity.unit << "." << std::endl;
        }
    }
    
    return std::min(gamma, 1.0); // Cap noise channel
}

} // namespace q_engine
