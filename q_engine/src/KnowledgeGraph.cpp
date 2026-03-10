#include "KnowledgeGraph.h"

namespace q_engine {

void KnowledgeGraph::record_attempt(ExprPtr expr, double loss, ErrorCategory et) {
    memory_.emplace_back(expr->to_string(), loss, et, clock_++);
}

bool KnowledgeGraph::has_failed_before(ExprPtr expr) const {
    std::string eq = expr->to_string();
    for (const auto& record : memory_) {
        if (record.equation_hash == eq && record.error_type != ErrorCategory::SUCCESS) {
            return true;
        }
    }
    return false;
}

double KnowledgeGraph::get_historical_penalty(ExprPtr expr) const {
    std::string eq = expr->to_string();
    double penalty = 0.0;
    
    // Simplest way to penalize: if we tried it and it died, penalize based on death type
    // In a fuller version, we would check structural similarity (AST subtrees matching known bad regions)
    for (const auto& record : memory_) {
        if (record.equation_hash == eq) {
            penalty += ErrorClassifier::compute_penalty(record.error_type);
        }
    }
    return penalty;
}

} // namespace q_engine
