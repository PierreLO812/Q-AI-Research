# Q-AI-Research (Q-Engine)

**Q-AI-Research** is an advanced AI-driven Quantum Physics engine written in high-performance C++. It is designed to autonomously discover, simulate, and mathematically verify new laws of quantum physics and wave mechanics. A user inputs an hypothesis, and the engine evaluates it through simulation, symbolic regression, and formal mathematical proof.

## 🧠 Architecture Overview (The 6 Stages of Cognition)

The system is built upon 6 powerful cognitive modules:

1. **Noyau Quantique (Simulation Core)**
   - High-performance C++ engine leveraging `Eigen` (BLAS/LAPACK) for dense matrix operations.
   - Solves the Lindblad Master Equation via RK4 to simulate quantum probability dissipation over time.
   - Applies Kraus operator maps to model environmental noise (NISQ).

2. **La Moelle Épinière (Physics-Informed Neural Networks - PINNs)**
   - Custom-built exact Reverse-Mode Automatic Differentiation (AD) engine.
   - Neural Networks natively embed the Schrödinger equation into their loss function, penalizing physics-violating predictions.

3. **Le Cerveau (Symbolic Regression & Discovery)**
   - **Al-Hilbert Algorithm**: Heuristically retro-engineers physical rules and mathematical relations by searching an Abstract Syntax Tree (AST) space.
   - **Positivstellensatz (SOS) Validator**: Acts as an infallible mathematical censor, automatically rejecting proposed laws that violate basic probability axioms (e.g., negative probabilities).

4. **La Mémoire (Temporal Knowledge Graph & Active Inference)**
   - Records all tested hypotheses and their failure modes (NISQ Noise, Divergence, SOS Violation).
   - Solves the Free Energy minimization problem to aggressively push the AI to explore highly uncertain, novel mathematical angles instead of repeating past failures.

5. **Le Juge (Formal Proof via Lean 4)**
   - The absolute standard of verification. Translated ASTs are pushed to a `Lean 4` Theorem Prover.
   - **RLVR (Reinforcement Learning from Verifier Feedback) Agent**: Learns to apply Lean tactics (`simp`, `ring`, `linarith`) to prove discoveries correct. If Lean fails to compile the proof, the hypothesis is scientifically rejected.

6. **L'Oracle (Execution & Interaction)**
   - The global executable integrating all phases.
   - Features a custom **Model Context Protocol (MCP)** bridge, returning findings in JSON format for consumption by standard language models.
   - Generates raw `<svg>` schemas (like Bloch Spheres) and exhaustive mathematical reports (`report.md`).

## 🚀 Building and Running

This project uses CMake and requires a C++14 compatible compiler (e.g., `g++` or MSVC). Note: The engine automates downloading its dependency `Eigen 3.4.0`.

```bash
# 1. Clone the repository
git clone https://github.com/PierreLO812/Q-AI-Research.git
cd Q-AI-Research/q_engine

# 2. Build the project
cmake -S . -B build
cmake --build build --config Release

# 3. Running L'Oracle
# You can execute the pipeline manually or directly feed it a hypothesis:
echo "Does quantum drift follow an exponential curve?" | ./start.exe
```

*Upon execution, the engine will run the 6 phases and generate a detailed `report.md` alongside an SVG visualization and JSON metadata.*

## 🧪 Testing the Modules

Dedicated test executives are available for each cognitive lobe of the Engine:
- `test_quantum.exe`: Validates the Lindblad solver and trace-preservation.
- `test_pinn.exe`: Validates the AutoDiff engine and physics-loss optimization.
- `test_cerveau.exe`: Validates AST generation, regression, and SOS rejection.
- `test_memoire.exe`: Validates curiosity bonuses and failure classification.
- `test_lean_juge.exe`: Validates parsing ASTs to Lean 4, compiling proofs, and catching false assertions.
