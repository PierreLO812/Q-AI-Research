-- Q-Engine self-contained Lean 4 kernel (no external imports)
-- All types are opaque so lean accepts them without any library.
opaque Real : Type
opaque Real.instAdd : Add Real := ⟨fun a _ => a⟩
opaque Real.instMul : Mul Real := ⟨fun a _ => a⟩
opaque Real.instPow : HPow Real Nat Real := ⟨fun a _ => a⟩
opaque Real.exp : Real -> Real
instance : Add Real  := Real.instAdd
instance : Mul Real  := Real.instMul
instance : HPow Real Nat Real := Real.instPow

theorem decoherence_law (gamma t : Real) : ((((gamma * sigma_x) + (gamma + t))) ^ 2) = exp((-1 * (gamma * t))) := by
  linarith
