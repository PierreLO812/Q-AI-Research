#ifndef ACTIVE_INFERENCE_H
#define ACTIVE_INFERENCE_H

#include "Symbolic.h"
#include "KnowledgeGraph.h"

namespace q_engine {

class ActiveInference {
public:
    // How much bonus do we give an equation for being completely UNKNOWN and UNTESTED
    // The less we know about an equation, the higher its Curiosity Reward
    static double compute_curiosity_reward(ExprPtr expr, const KnowledgeGraph& memory);

    // Compute the Active Inference Free Energy:
    // F = (Expected Accuracy Loss) + (Historical Penalty) - (Curiosity Reward)
    // The optimizer's goal is to minimize F.
    static double compute_free_energy(ExprPtr expr, double current_loss, bool sos_valid, const KnowledgeGraph& memory);
};

} // namespace q_engine

#endif
