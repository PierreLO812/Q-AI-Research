# 🌌 Q-AI-Research: The Engine of Quantum Discovery

**Q-AI-Research** is a state-of-the-art, high-performance C++ discovery engine designed to autonomously bridge the gap between quantum simulation and formal mathematical proofs. By combining **Lindblad Physics**, **Symbolic Regression (SINDy)**, and **Formal Verification (Lean 4)**, it doesn't just simulate quantum systems—it *derives their fundamental laws*.

---

## 🏛️ Architecture: The 6 Pillars of L'Oracle

The engine operates on a native, zero-mock pipeline:

### 1. 📡 Semantic Perception (NLP)
Parses human-language hypotheses (e.g., *"How does noise affect drift?"*) into structured physical entities and constraints.

### 2. 🧪 Quantum Simulation Core
Solves the **Lindblad Master Equation** via high-performance C++ (Eigen) to generate ground-truth fidelity trajectories under NISQ environmental noise.

### 3. 🧠 Le Cerveau (Symbolic Regression)
- **Al-Hilbert**: Explores Abstract Syntax Tree (AST) spaces to find candidate equations.
- **PySINDy Integration**: Offloads high-dimensional regression to native Python workers using Numpy Polyfit for precise coefficient discovery.

### 4. ⚖️ PG-SR & Z3 Validation
- **Dimensional Analysis (PG-SR)**: Rejects any equation that violates SI unit coherence.
- **Z3 SMT Solver**: Formally verifies that discovering parameters satisfy physical bounds and transcendental constraints.

### 5. ⚖️ Le Juge (Native Lean 4 Verification)
- **RLVR (Reinforcement Learning from Verifier feedback)**: Automatically translates ASTs into Lean 4 theorems.
- **Tactic Loop**: Invokes a native Lean 4 kernel to prove the discovered law matches the sim-data. It captures real compiler errors to guide mutations.

### 6. 🦾 LLM-in-the-Loop Mutator
When a proof fails, a **local Keep-Alive LLM** (Safetensors) analyzes the exact Lean 4 error and the current AST to suggest surgical mutations, breaking the discovery bottleneck.

---

## ⚡ Quick Start

### Prerequisites
- **C++17 Compiler** (MSVC 2022 / GCC 11+)
- **CMake 3.20+**
- **Python 3.10+** (with `numpy`)
- **Lean 4** (via `elan`)

### Building
```powershell
# Navigate to engine
cd q_engine

# Configure & Build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Copy binaries
copy build\start.exe .
```

### Running
```powershell
.\start.exe
```
*Enter your hypothesis and watch the engine iterate through 5 mutation loops to converge on a physical law.*

---

## 📜 Documentation
- [USERGUIDE.md](USERGUIDE.md): Deep dive into logs, error codes, and workflow internals.
- [ARCHITECTURE.md](docs/ARCHITECTURE.md): Technical whitepaper on RLVR and SOS Certificates.

---

## 🛸 Visual Output
The engine generates a real-time `report.md` with:
- **Verified Physics Laws**
- **Bloch Sphere `<svg>` Visualizations**
- **Lean 4 Proof Steps**
- **Curiosity impasses from the Temporal Knowledge Graph**
