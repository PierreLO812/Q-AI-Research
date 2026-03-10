#include "ActiveInference.h"
#include <cmath>

namespace q_engine {

double ActiveInference::compute_curiosity_reward(ExprPtr expr, const KnowledgeGraph& memory) {
    if (memory.has_failed_before(expr)) {
        return 0.0; // No curiosity for a known dead end 
    }
    // High curiosity reward to encourage trying something totally unrecorded
    return 50.0;
}

double ActiveInference::compute_free_energy(ExprPtr expr, double current_loss, bool sos_valid, const KnowledgeGraph& memory) {
    // 1. Analyze the mathematical state
    ErrorCategory ec = ErrorClassifier::analyze_failure(expr, current_loss, sos_valid);
    
    // 2. Base Expected Loss
    double accuracy_loss = current_loss;
    if (ec != ErrorCategory::SUCCESS) {
        accuracy_loss += ErrorClassifier::compute_penalty(ec);
    }
    
    // 3. Historical penalty (Has this exact sub-tree failed before?)
    double historical_penalty = memory.get_historical_penalty(expr);
    
    // 4. Curiosity (Have we explored this mathematical territory?)
    double curiosity_reward = compute_curiosity_reward(expr, memory);
    
    // Free Energy F = Loss + History_Penalty - Curiosity
    // The Active Inference engine seeks to minimize F.
    // By doing so, it naturally avoids History_Penalty and gravitates towards high Curiosity 
    // when accuracy is flat!
    return accuracy_loss + historical_penalty - curiosity_reward;
}

} // namespace q_engine
