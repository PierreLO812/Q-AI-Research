#include "KnowledgeGraph.h"

namespace q_engine {

void KnowledgeGraph::record_attempt(ExprPtr expr, double loss, ErrorCategory et) {
    memory_.emplace_back(expr->to_string(), loss, et, clock_++);
}

void KnowledgeGraph::record_mutation_attempt(ExprPtr rejected_expr, const std::string& lean_log, const std::string& mutation_json) {
    // Record the fact that this equation was rejected and LLM suggested the mutation_json
    memory_.emplace_back(rejected_expr->to_string(), 1.0, ErrorCategory::DIVERGENCE, clock_++, mutation_json);
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
