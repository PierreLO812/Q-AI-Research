#ifndef PINN_H
#define PINN_H

#include "AutoDiff.h"
#include <vector>
#include <random>
#include <iostream>

namespace q_engine {

// A clever struct that traces physical equations natively!
// Instead of a massive graph, we explicitly forward the 1st and 2nd derivatives.
struct PhysVar {
    ValuePtr val;
    ValuePtr dx; // d / dx
    ValuePtr dt; // d / dt
    ValuePtr dxx; // d^2 / dx^2

    PhysVar() {
        val = make_value(0.0);
        dx = make_value(0.0);
        dt = make_value(0.0);
        dxx = make_value(0.0);
    }
};

class Layer {
public:
    std::vector<std::vector<ValuePtr>> W;
    std::vector<ValuePtr> b;

    Layer(int in_features, int out_features) {
        std::mt19937 gen(42);
        std::normal_distribution<double> d(0.0, 0.1);

        for (int i = 0; i < out_features; ++i) {
            std::vector<ValuePtr> row;
            for (int j = 0; j < in_features; ++j) {
                row.push_back(make_value(d(gen)));
            }
            W.push_back(row);
            b.push_back(make_value(0.0));
        }
    }

    std::vector<ValuePtr> parameters() const {
        std::vector<ValuePtr> params;
        for (const auto& row : W) {
            for (const auto& w : row) {
                params.push_back(w);
            }
        }
        for (const auto& bias : b) {
            params.push_back(bias);
        }
        return params;
    }

    std::vector<PhysVar> forward(const std::vector<PhysVar>& inputs, bool use_activation = true) {
        std::vector<PhysVar> outputs(W.size());
        for (size_t i = 0; i < W.size(); ++i) {
            ValuePtr sum_val = make_value(0.0);
            ValuePtr sum_dx = make_value(0.0);
            ValuePtr sum_dt = make_value(0.0);
            ValuePtr sum_dxx = make_value(0.0);

            for (size_t j = 0; j < inputs.size(); ++j) {
                sum_val = sum_val + W[i][j] * inputs[j].val;
                sum_dx = sum_dx + W[i][j] * inputs[j].dx;
                sum_dt = sum_dt + W[i][j] * inputs[j].dt;
                sum_dxx = sum_dxx + W[i][j] * inputs[j].dxx;
            }
            sum_val = sum_val + b[i];

            if (use_activation) {
                ValuePtr act_val = tanh(sum_val);
                ValuePtr tanh_sq = pow2(act_val);
                ValuePtr sec_sq = make_value(1.0) - tanh_sq; 

                outputs[i].val = act_val;
                outputs[i].dx = sec_sq * sum_dx; // chain rule for first derivative
                outputs[i].dt = sec_sq * sum_dt;
                
                // Chain rule for second order derivative
                ValuePtr first_term = make_value(-2.0) * act_val * sec_sq * pow2(sum_dx);
                ValuePtr second_term = sec_sq * sum_dxx;
                outputs[i].dxx = first_term + second_term;
            } else {
                outputs[i].val = sum_val;
                outputs[i].dx = sum_dx;
                outputs[i].dt = sum_dt;
                outputs[i].dxx = sum_dxx;
            }
        }
        return outputs;
    }
};

class PhysicsNetwork {
public:
    std::vector<Layer> layers;

    PhysicsNetwork() {
        layers.push_back(Layer(2, 20)); // inputs: x, t
        layers.push_back(Layer(20, 20));
        layers.push_back(Layer(20, 2)); // outputs: Real(psi), Imag(psi)
    }

    std::vector<ValuePtr> parameters() const {
        std::vector<ValuePtr> params;
        for (const auto& layer : layers) {
            auto p = layer.parameters();
            params.insert(params.end(), p.begin(), p.end());
        }
        return params;
    }

    std::vector<PhysVar> forward(ValuePtr x, ValuePtr t) {
        PhysVar vx, vt;
        vx.val = x;
        vx.dx = make_value(1.0); // derivative of x wrt x is 1
        vx.dt = make_value(0.0);
        vx.dxx = make_value(0.0);

        vt.val = t;
        vt.dx = make_value(0.0);
        vt.dt = make_value(1.0); // derivative of t wrt t is 1
        vt.dxx = make_value(0.0);

        std::vector<PhysVar> current = {vx, vt};
        for (size_t i = 0; i < layers.size(); ++i) {
            bool use_act = (i != layers.size() - 1);
            current = layers[i].forward(current, use_act);
        }
        return current;
    }
};

} // namespace q_engine

#endif
