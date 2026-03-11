cmake_minimum_required(VERSION 3.14)
project(QuantumEngine CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG        3.4.0
)
FetchContent_MakeAvailable(eigen)

# --- Z3 Native (Phase 8 Master Mode) ---
# We mock Z3 bindings, no Pybind11 anymore as C++ is the absolute master orchestrator

add_library(q_engine src/QuantumCore.cpp src/AutoDiff.cpp src/Symbolic.cpp src/AlHilbert.cpp src/Positivstellensatz.cpp src/ErrorClassifier.cpp src/KnowledgeGraph.cpp src/ActiveInference.cpp src/LeanTranslator.cpp src/LeanInterface.cpp src/RLVR_Agent.cpp src/SchemaGenerator.cpp src/MCPBridge.cpp src/NLParser.cpp src/ConstraintBuilder.cpp src/Z3Validator.cpp src/PythonBridge.cpp)
target_include_directories(q_engine PUBLIC include)
target_link_libraries(q_engine PUBLIC Eigen3::Eigen)

enable_testing()
add_executable(test_quantum tests/test_quantum.cpp)
target_link_libraries(test_quantum PRIVATE q_engine)

add_test(NAME test_quantum COMMAND test_quantum)

add_executable(test_pinn tests/test_pinn.cpp)
target_link_libraries(test_pinn PRIVATE q_engine)
add_test(NAME test_pinn COMMAND test_pinn)

add_executable(test_cerveau tests/test_cerveau.cpp)
target_link_libraries(test_cerveau PRIVATE q_engine)
add_test(NAME test_cerveau COMMAND test_cerveau)

add_executable(test_memoire tests/test_memoire.cpp)
target_link_libraries(test_memoire PRIVATE q_engine)
add_test(NAME test_memoire COMMAND test_memoire)

add_executable(test_lean_juge tests/test_lean_juge.cpp)
target_link_libraries(test_lean_juge PRIVATE q_engine)
add_test(NAME test_lean_juge COMMAND test_lean_juge)

add_executable(start src/main.cpp)
target_link_libraries(start PRIVATE q_engine)
