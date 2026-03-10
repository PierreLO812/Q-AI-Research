#ifndef LEAN_TRANSLATOR_H
#define LEAN_TRANSLATOR_H

#include "Symbolic.h"
#include <string>

namespace q_engine {

class LeanTranslator {
public:
    // Translates an AST expression to a Lean 4 string representation.
    static std::string translate_expression(ExprPtr expr);

    // Generates a complete Lean 4 file content for a theorem.
    static std::string generate_theorem_file(const std::string& theorem_name, ExprPtr lhs, ExprPtr rhs, const std::string& tactic = "sorry");
};

} // namespace q_engine

#endif
