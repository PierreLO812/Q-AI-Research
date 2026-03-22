#ifndef LLM_MUTATOR_H
#define LLM_MUTATOR_H

#include "Symbolic.h"
#include <string>
#include <memory>

// Forward declarations for llama.cpp types to avoid including llama.h in the header
struct llama_model;
struct llama_context;

namespace q_engine {

class LLMMutator {
public:
    // Singleton access
    static LLMMutator& get_instance() {
        static LLMMutator instance;
        return instance;
    }

    // Initialize the Keep-Alive context
    // This loads the safetensors model and tekken format from the given directory
    bool init_keep_alive(const std::string& model_dir);

    // Main inference loop logic: Formats prompt, inferences, parses JSON, mutates AST
    ExprPtr mutate_ast(ExprPtr failed_equation, const std::string& lean_log);

private:
    LLMMutator() = default;
    ~LLMMutator();

    // Disallow copies
    LLMMutator(const LLMMutator&) = delete;
    LLMMutator& operator=(const LLMMutator&) = delete;

    std::string generate_text(const std::string& formatted_prompt);
    
    // Hardcoded System Prompt
    const std::string system_prompt = 
        "Tu es un mutateur d'Arbre Syntaxique Abstrait (AST) pour un moteur de physique quantique. "
        "Rejet formel reçu. Analyse l'erreur de type ou d'unitarité. "
        "Renvoie EXCLUSIVEMENT un JSON valide indiquant le composant à remplacer. "
        "Interdiction absolue d'écrire du texte ou des explications en dehors du JSON.";

    llama_model* model_ = nullptr;
    llama_context* ctx_  = nullptr;
    bool initialized_ = false;

    // State memory for Active Inference (historique des mutations)
    std::vector<std::string> mutation_history_;
};

} // namespace q_engine

#endif
