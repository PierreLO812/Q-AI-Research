#ifndef MCP_BRIDGE_H
#define MCP_BRIDGE_H

#include <string>

namespace q_engine {

class MCPBridge {
public:
    // Takes in an hypothesis string and formats the pipeline output as JSON
    // for ingestion by a Large Language Model.
    static std::string process_hypothesis_to_json(const std::string& hypothesis, bool verified, double confidence, const std::string& equation, const std::string& lean_error="");
};

} // namespace q_engine

#endif
