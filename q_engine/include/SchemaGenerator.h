#ifndef SCHEMA_GENERATOR_H
#define SCHEMA_GENERATOR_H

#include "Symbolic.h"
#include <string>

namespace q_engine {

class SchemaGenerator {
public:
    // Generate a beautiful SVG representation of an equation tree
    static std::string generate_equation_svg(ExprPtr expr);
    
    // Generate an abstract SVG representing a 3D Bloch Sphere
    static std::string generate_bloch_sphere_svg(double prob_0, double prob_1);
};

} // namespace q_engine

#endif
