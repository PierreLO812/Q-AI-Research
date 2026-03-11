#ifndef CONSTRAINT_BUILDER_H
#define CONSTRAINT_BUILDER_H

#include "NLParser.h"
#include "Symbolic.h"
#include <vector>

namespace q_engine {

class ConstraintBuilder {
public:
    // Transforms a set of natural language parameters into a mathematical constraint Expression Tree (AST)
    static ExprPtr build_fidelity_constraint(const std::vector<PhysicalEntity>& entities);
    
    // Calculates the required noise coherence factor limit based on physical inputs
    static double calculate_gamma_from_entities(const std::vector<PhysicalEntity>& entities);
};

} // namespace q_engine

#endif
