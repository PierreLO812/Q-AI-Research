#ifndef DIMENSIONAL_ANALYSIS_H
#define DIMENSIONAL_ANALYSIS_H

#include "Symbolic.h"
#include "NLParser.h"
#include <string>
#include <map>
#include <vector>

namespace q_engine {

// ═══════════════════════════════════════════════════════════════
// PG-SR DIMENSIONAL ANALYSIS — Phase 9 Action 3
// Rejects dimensionally incoherent equations before Lean 4 call.
// e.g. "distance + pressure" → REJECTED immediately.
// ═══════════════════════════════════════════════════════════════

class DimensionalAnalysis {
public:
    // Tags all variables extracted from the hypothesis with their units
    static std::map<std::string, std::string> build_dimension_map(const std::vector<PhysicalEntity>& entities);

    // Checks dimensional coherence of an expression tree.
    // Returns the resulting dimension string ("m", "G", "dimensionless" etc.) or "INCOHERENT".
    static std::string check_dimensions(ExprPtr expr, const std::map<std::string, std::string>& dim_map);

    // Returns true only if expression is dimensionally valid
    static bool is_dimensionally_valid(ExprPtr expr, const std::map<std::string, std::string>& dim_map);

private:
    static bool dimensions_compatible(const std::string& d1, const std::string& d2);
};

} // namespace q_engine

#endif
