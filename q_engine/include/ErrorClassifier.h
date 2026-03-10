#ifndef ERROR_CLASSIFIER_H
#define ERROR_CLASSIFIER_H

#include "Symbolic.h"

namespace q_engine {

enum class ErrorCategory {
    SUCCESS,
    NISQ_NOISE,     // Environmental noise too destructive
    DIVERGENCE,     // Mathematical NaN/Inf explosion
    SOS_VIOLATION   // Positivstellensatz failed
};

class ErrorClassifier {
public:
    static ErrorCategory analyze_failure(ExprPtr expr, double loss, bool sos_valid);
    static double compute_penalty(ErrorCategory category);
};

} // namespace q_engine

#endif
