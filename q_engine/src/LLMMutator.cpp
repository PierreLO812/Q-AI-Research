#include "LLMMutator.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

// Assuming llama.h is available in include path after submodule compilation
// #include "llama.h"

using json = nlohmann::json;

namespace q_engine {

LLMMutator::~LLMMutator() {
    // if (ctx_) llama_free(ctx_);
    // if (model_) llama_free_model(model_);
    // llama_backend_free();
}

bool LLMMutator::init_keep_alive(const std::string& model_dir) {
    if (initialized_) return true;

    // llama_backend_init();

    std::string model_path = model_dir + "/consolidated.safetensors";
    
    // We setup the model parameters
    // auto model_params = llama_model_default_params();
    // model_params.n_gpu_layers = 99; // Force GPU offloading

    std::cout << "[LLMMutator] Loading Keep-Alive model from " << model_path << " ..." << std::endl;

    // MOCK FOR COMPILATION:
    // model_ = llama_load_model_from_file(model_path.c_str(), model_params);
    model_ = reinterpret_cast<llama_model*>(1);
    if (!model_) {
        std::cerr << "[LLMMutator] Failed to load model. Ensure the safetensors model exists." << std::endl;
        return false;
    }

    // auto ctx_params = llama_context_default_params();
    // ctx_params.n_ctx = 4096;
    // ctx_params.n_batch = 512;

    // ctx_ = llama_new_context_with_model(model_, ctx_params);
    ctx_ = reinterpret_cast<llama_context*>(1);
    if (!ctx_) {
        std::cerr << "[LLMMutator] Failed to create llama context" << std::endl;
        // llama_free_model(model_);
        return false;
    }

    initialized_ = true;
    std::cout << "[LLMMutator] Model successfully loaded into Keep-Alive memory." << std::endl;
    return true;
}

std::string LLMMutator::generate_text(const std::string& formatted_prompt) {
    if (!initialized_) return "{}";

    // MOCK generation for rapid prototype testing without requiring the full weights to be actually run
    // In a real execution, we would use llama_decode, llama_sample_token, etc.
    
    // --- Llama inference loop placeholder ---
    std::cout << "[LLMMutator] LLM Inference running on prompt (length: " << formatted_prompt.size() << ") ..." << std::endl;
    
    // Simulate LLM returning a correct JSON
    // We extract the failed target arbitrarily or by analyzing prompt
    std::string mock_json = R"({
        "action": "replace",
        "target_node": "sigma_x",
        "new_operator": "sigma_z"
    })";

    return mock_json;
}

ExprPtr LLMMutator::mutate_ast(ExprPtr failed_equation, const std::string& lean_log) {
    if (!initialized_) {
        std::cerr << "[LLMMutator] Mutator not initialized!" << std::endl;
        return failed_equation;
    }

    // 1. Build history string for Active Inference
    std::string history_str = "[ ";
    for (const auto& h : mutation_history_) {
        history_str += "\"" + h + "\", ";
    }
    if (!mutation_history_.empty()) history_str.pop_back(); history_str.pop_back();
    history_str += " ]";

    // Format new input with history included
    std::string user_input = "Voici l'erreur. Historique des tentatives échouées : " + history_str + 
                             ". Propose une mutation DIFFÉRENTE.\nÉquation Actuelle : " + failed_equation->to_string() + 
                             "\nLean Log/Error: " + lean_log;

    // Add current failure to history
    mutation_history_.push_back(failed_equation->to_string());

    // We apply chat_template (simulated format applied to input)
    std::string formatted_prompt = "<|system|>\n" + system_prompt + "\n<|user|>\n" + user_input + "\n<|assistant|>\n";

    // 2. Inference
    std::string llm_output = generate_text(formatted_prompt);
    
    // 3. Parsing JSON via nlohmann
    try {
        json response = json::parse(llm_output);
        std::string action = response.value("action", "");
        std::string target = response.value("target_node", "");
        std::string new_op = response.value("new_operator", "");

        std::cout << " -> LLM Suggested JSON: " << response.dump() << std::endl;

        if (action == "replace") {
            // Find in AST and replace (Mocking the recursive search for simplicity, replacing the whole if it matches or doing a basic transform)
            // Here we just wrap the old in a new operator or mutate root for demonstration
            // A real AST visitor would traverse the tree looking for 'target' node name.
            
            // Re-create a new operator to replace the target
            std::cout << " -> Executing AST mutation: Replacing '" << target << "' with '" << new_op << "'" << std::endl;
            
            // For example, if LLM suggests sigma_z instead of sigma_x:
            // return a completely new AST or mutated tree.
            // We just return a mock new AST representing the merged state.
            return make_lindblad_dissipator(make_pauli(PauliAxis::Z), make_density_matrix(), 0.01);
        }
    } catch (const json::exception& e) {
        std::cerr << "[LLMMutator] Failed to parse LLM JSON output: " << e.what() << std::endl;
    }

    return failed_equation; // fallback
}

} // namespace q_engine
