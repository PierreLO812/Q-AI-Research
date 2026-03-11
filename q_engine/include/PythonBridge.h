#ifndef PYTHON_BRIDGE_H
#define PYTHON_BRIDGE_H

#include <string>
#include "Symbolic.h" // For ExprPtr

namespace q_engine {

class PythonBridge {
public:
    // Execute a Python worker script, passing data as arguments or via JSON,
    // and capturing the stdout to extract the resulting mathematical equation or boolean result.
    static ExprPtr execute_worker_sindy(const std::string& script_path, const std::string& simulation_data_json);
    
    // Example worker for another hypothetical framework (DeepXDE)
    static std::string execute_worker_deepxde(const std::string& script_path, double gamma_noise);

private:
    // Helper function to execute a system command and securely capture stdout.
    static std::string exec_python_script(const std::string& command);
};

} // namespace q_engine

#endif
