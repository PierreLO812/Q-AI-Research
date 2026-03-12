#include "DimensionalAnalysis.h"
#include <iostream>

namespace q_engine {

std::map<std::string, std::string> DimensionalAnalysis::build_dimension_map(const std::vector<PhysicalEntity>& entities) {
    std::map<std::string, std::string> dim_map;
    for (const auto& e : entities) {
        dim_map[e.type] = e.unit; // ex: map["distance"] = "m", map["pressure"] = "G"
    }
    // Always include standard quantum variables as dimensionless
    dim_map["gamma"]  = "dimensionless"; // dissipation rate (normalized)
    dim_map["t"]      = "s";             // time
    dim_map["hbar"]   = "J.s";           // Planck constant
    dim_map["omega"]  = "Hz";            // angular frequency
    dim_map["F"]      = "dimensionless"; // Fidelity (always dimensionless)
    dim_map["rho"]    = "dimensionless"; // Density matrix (dimensionless operator)
    return dim_map;
}

bool DimensionalAnalysis::dimensions_compatible(const std::string& d1, const std::string& d2) {
    if (d1 == d2) return true;
    if (d1 == "dimensionless" || d2 == "dimensionless") return true;
    if (d1 == "INCOHERENT" || d2 == "INCOHERENT") return false;
    return false; // Strict: mismatched physical units are incompatible
}

std::string DimensionalAnalysis::check_dimensions(ExprPtr expr, const std::map<std::string, std::string>& dim_map) {
    if (!expr) return "dimensionless";
    
    switch (expr->type) {
        case NodeType::CONSTANT: return "dimensionless";
        case NodeType::VARIABLE: {
            auto it = dim_map.find(expr->name);
            if (it != dim_map.end()) return it->second;
            return "dimensionless"; // Unknown variable: assume dimensionless
        }
        case NodeType::ADD:
        case NodeType::SUB: {
            std::string dl = check_dimensions(expr->left,  dim_map);
            std::string dr = check_dimensions(expr->right, dim_map);
            if (!dimensions_compatible(dl, dr)) {
                std::cout << "   [PG-SR DIM] REJECT: Cannot add/sub [" << dl << "] and [" << dr << "]" << std::endl;
                return "INCOHERENT";
            }
            return dl == "dimensionless" ? dr : dl;
        }
        case NodeType::MUL: {
            std::string dl = check_dimensions(expr->left,  dim_map);
            std::string dr = check_dimensions(expr->right, dim_map);
            if (dl == "INCOHERENT" || dr == "INCOHERENT") return "INCOHERENT";
            // Multiplication combines dimensions (e.g. m * m = m^2)
            if (dl == "dimensionless") return dr;
            if (dr == "dimensionless") return dl;
            return dl + "." + dr; // e.g. "m.s"
        }
        case NodeType::SQUARE:
            return check_dimensions(expr->left, dim_map); // Same unit squared is fine
        case NodeType::EXP:
        case NodeType::TANH:
        case NodeType::COMPLEX_EXP:
            // Argument must be dimensionless
            if (check_dimensions(expr->left, dim_map) != "dimensionless") {
                std::cout << "   [PG-SR DIM] REJECT: Transcendental argument must be dimensionless." << std::endl;
                return "INCOHERENT";
            }
            return "dimensionless";
        // Quantum operators produce dimensionless operators on Hilbert space
        case NodeType::PAULI:
        case NodeType::COMMUTATOR:
        case NodeType::ANTICOMMUTATOR:
        case NodeType::DAGGER:
        case NodeType::LINDBLAD_DISSIPATOR:
        case NodeType::KRAUS_CHANNEL:
        case NodeType::TENSOR_PRODUCT:
        case NodeType::PARTIAL_TRACE:
        case NodeType::DENSITY_MATRIX:
            return "dimensionless"; // Quantum operators are dimensionless by convention
        default:
            return "dimensionless";
    }
}

bool DimensionalAnalysis::is_dimensionally_valid(ExprPtr expr, const std::map<std::string, std::string>& dim_map) {
    return check_dimensions(expr, dim_map) != "INCOHERENT";
}

} // namespace q_engine
