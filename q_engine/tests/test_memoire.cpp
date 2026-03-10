#include "ActiveInference.h"
#include "Positivstellensatz.h"
#include <iostream>

using namespace q_engine;

int main() {
    std::cout << "Testing La Memoire & Active Inference..." << std::endl;

    KnowledgeGraph memory;

    // Suppose the AI generated this "known bad" equation x^2 - 100
    // And Positivstellensatz rightly fails it.
    ExprPtr bad_expr = make_sub(make_square(make_var("x")), make_const(100.0));
    
    // Simulate finding a failure
    double fake_loss = 250.0;
    bool sos_valid = CertificateSOS::verify(bad_expr); // False
    
    ErrorCategory ec = ErrorClassifier::analyze_failure(bad_expr, fake_loss, sos_valid);
    
    // Verify Classifier
    if (ec == ErrorCategory::SOS_VIOLATION) {
        std::cout << "[SUCCESS] ErrorClassifier correctly identified Positivstellensatz violation." << std::endl;
    } else {
        std::cerr << "[ERROR] ErrorClassifier failed to identify SOS violation." << std::endl;
        return 1;
    }
    
    // Record to memory
    memory.record_attempt(bad_expr, fake_loss, ec);
    
    // Now simulate Active Inference during the next generation
    // Option A: Try the bad law again
    double free_energy_a = ActiveInference::compute_free_energy(bad_expr, fake_loss, false, memory);
    
    // Option B: Try a completely new, unknown safe equation: x^2 + 10
    ExprPtr good_new_expr = make_add(make_square(make_var("x")), make_const(10.0));
    double fake_loss_b = 300.0; // Imagine this one currently fits the data *worse*
    bool sos_b = CertificateSOS::verify(good_new_expr); // True
    
    double free_energy_b = ActiveInference::compute_free_energy(good_new_expr, fake_loss_b, sos_b, memory);
    
    std::cout << "Free Energy (Try Known Failed Path): " << free_energy_a << std::endl;
    std::cout << "Free Energy (Try Novel Safe Path): " << free_energy_b << std::endl;

    // The AI should prefer Option B (Lowest Free Energy)
    if (free_energy_b < free_energy_a) {
        std::cout << "[SUCCESS] Active Inference successfully repelled AI from the trap and incentivized curiosity!" << std::endl;
    } else {
        std::cerr << "[ERROR] Active Inference failed to protect the AI." << std::endl;
        return 1;
    }
    
    return 0;
}
