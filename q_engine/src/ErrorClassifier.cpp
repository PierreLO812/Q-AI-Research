#include "ErrorClassifier.h"
#include <cmath>

namespace q_engine {

ErrorCategory ErrorClassifier::analyze_failure(ExprPtr expr, double loss, bool sos_valid) {
    if (!sos_valid) {
        return ErrorCategory::SOS_VIOLATION;
    }
    if (std::isnan(loss) || std::isinf(loss) || loss > 1e15) {
        return ErrorCategory::DIVERGENCE;
    }
    if (loss > 100.0) { // Arbitrary threshold representing overwhelming loss due to noise
        return ErrorCategory::NISQ_NOISE;
    }
    return ErrorCategory::SUCCESS;
}

double ErrorClassifier::compute_penalty(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::SOS_VIOLATION: return 1e9; // Fatal, absolute block
        case ErrorCategory::DIVERGENCE: return 1e6;    // Extremely bad, mathematical singularity
        case ErrorCategory::NISQ_NOISE: return 1e3;    // Bad, but physically possible (just terrible coherence)
        case ErrorCategory::SUCCESS: return 0.0;
    }
    return 0.0;
}

} // namespace q_engine
