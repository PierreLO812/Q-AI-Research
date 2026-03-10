#ifndef POSITIVSTELLENSATZ_H
#define POSITIVSTELLENSATZ_H

#include "Symbolic.h"

namespace q_engine {

class CertificateSOS {
public:
    // Simple heuristic SOS checker:
    // Returns true if the expression tree is structurally guaranteed to be >= 0
    // Example: (x)^2 + (y)^2 is always >= 0, and constants >= 0 are >= 0.
    static bool verify(ExprPtr expr);
};

} // namespace q_engine

#endif
