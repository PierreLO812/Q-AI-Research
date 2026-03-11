#ifndef Z3_VALIDATOR_H
#define Z3_VALIDATOR_H

#include "Symbolic.h"
#include <string>

namespace q_engine {

class Z3Validator {
public:
    // Transforms an Abstract Syntax Tree into Z3 SMT constraints and checks satisfiability.
    // Returns true if the equation is logically and physically satisfiable under given boundaries.
    static bool verify_satisfiability(ExprPtr equation);
};

} // namespace q_engine

#endif
