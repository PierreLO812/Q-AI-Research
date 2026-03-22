#ifndef KNOWLEDGE_GRAPH_H
#define KNOWLEDGE_GRAPH_H

#include "Symbolic.h"
#include "ErrorClassifier.h"
#include <vector>
#include <string>

namespace q_engine {

struct SimulationRecord {
    std::string equation_hash;
    double loss;
    ErrorCategory error_type;
    int timestamp;
    std::string mutation_json; // Optionally store the JSON from LLM

    SimulationRecord(const std::string& eq, double l, ErrorCategory et, int t, const std::string& json = "")
        : equation_hash(eq), loss(l), error_type(et), timestamp(t), mutation_json(json) {}
};

class KnowledgeGraph {
public:
    // Store every single attempt
    void record_attempt(ExprPtr expr, double loss, ErrorCategory et);

    // Store LLM mutations
    void record_mutation_attempt(ExprPtr rejected_expr, const std::string& lean_log, const std::string& mutation_json);

    // Give me the history of an equation (exact match or structural)
    bool has_failed_before(ExprPtr expr) const;
    
    // Get the base penalty of a region in equation-space
    double get_historical_penalty(ExprPtr expr) const;

    // Give me the total number of simulations run
    int get_current_time() const { return clock_; }

private:
    std::vector<SimulationRecord> memory_;
    int clock_ = 0;
};

} // namespace q_engine

#endif
