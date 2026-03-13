#include "ConstraintBuilder.h"
#include "Symbolic.h"
#include <cmath>
#include <iostream>
#include <sstream>

namespace q_engine {

ExprPtr ConstraintBuilder::build_fidelity_constraint(const std::vector<PhysicalEntity>& entities) {
    double target_fidelity = 0.8;
    for (const auto& entity : entities) {
        if (entity.type == "distance" && entity.value > 50000.0) target_fidelity = 0.5;
        if (entity.type == "pressure" && entity.value > 1.5)    target_fidelity += 0.1;
    }
    return make_add(make_var("F"), make_const(-target_fidelity));
}

double ConstraintBuilder::calculate_gamma_from_entities(const std::vector<PhysicalEntity>& entities) {
    double gamma = 0.01;

    for (const auto& entity : entities) {
        if (entity.type == "distance") {
            // Fiber attenuation: 0.2 dB/km coupling factor
            // Physical dimension: [1/km] x [km] = dimensionless rate
            double attenuation = (entity.value / 1000.0) * 0.05;
            gamma += attenuation;
            // Fix 3: Annotate with physical unit and description
            auto annotated = make_physical_const(
                attenuation,
                "1/km",
                "Fiber attenuation rate (" + std::to_string(entity.value) + " m at 0.05 dB/km)"
            );
            std::cout << "   -> Fiber attenuation: " << annotated->to_string() << std::endl;
        }
        else if (entity.type == "pressure") {
            // G-Force induced birefringence Omega (rad/s)
            // Reference: pressure-induced birefringence coupling in SMF = 0.15 Omega/G
            double birefringence = entity.value * 0.15;
            gamma += birefringence;
            auto annotated = make_physical_const(
                birefringence,
                "Omega",
                "G-force birefringence coupling (" + std::to_string(entity.value) + " G x 0.15 Omega/G)"
            );
            std::cout << "   -> G-force dephasing: " << annotated->to_string() << std::endl;
        }
        else if (entity.type == "temperature") {
            double temp_c = entity.unit == "K" ? entity.value - 273.15 : entity.value;
            double thermal = (temp_c > 20.0 ? (temp_c - 20.0) * 0.005 : 0);
            gamma += thermal;
            auto annotated = make_physical_const(
                thermal,
                "K_B*T_excess",
                "Thermal noise from " + std::to_string(entity.value) + entity.unit + " environment"
            );
            std::cout << "   -> Thermal noise: " << annotated->to_string() << std::endl;
        }
    }
    return std::min(gamma, 1.0);
}

} // namespace q_engine
