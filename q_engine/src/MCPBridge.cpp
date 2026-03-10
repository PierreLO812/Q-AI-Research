#include "MCPBridge.h"
#include <sstream>

namespace q_engine {

std::string MCPBridge::process_hypothesis_to_json(const std::string& hypothesis, bool verified, double confidence, const std::string& equation, const std::string& lean_error) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"engine\": \"Q-Engine C++\",\n";
    json << "  \"hypothesis_processed\": \"" << hypothesis << "\",\n";
    json << "  \"results\": {\n";
    json << "    \"is_verified_mathematically\": " << (verified ? "true" : "false") << ",\n";
    json << "    \"confidence_score\": " << confidence << ",\n";
    json << "    \"discovered_equation\": \"" << equation << "\",\n";
    
    if (!verified && !lean_error.empty()) {
        json << "    \"rejection_reason\": \"" << lean_error << "\"\n";
    } else {
        json << "    \"rejection_reason\": null\n";
    }
    
    json << "  }\n";
    json << "}\n";
    
    return json.str();
}

} // namespace q_engine
