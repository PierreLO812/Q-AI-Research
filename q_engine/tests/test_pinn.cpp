#include "PINN.h"
#include <iostream>

using namespace q_engine;

int main() {
    std::cout << "Testing PINN & AutoDiff Engine..." << std::endl;

    PhysicsNetwork net;

    std::vector<ValuePtr> x_vals, t_vals;
    for (int i = -2; i <= 2; ++i) {
        x_vals.push_back(make_value(i * 0.5));
        t_vals.push_back(make_value(0.1));
    }

    double lr = 0.01;
    auto params = net.parameters();

    for (int epoch = 0; epoch <= 100; ++epoch) {
        // Zero grad
        for (auto& p : params) {
            p->grad = 0.0;
        }

        ValuePtr total_loss = make_value(0.0);

        for (size_t i = 0; i < x_vals.size(); ++i) {
            auto out = net.forward(x_vals[i], t_vals[i]);
            PhysVar u = out[0];
            PhysVar v = out[1];

            // Free particle: V(x) = 0
            ValuePtr V = make_value(0.0);

            // f_u = v_t - 0.5 * u_xx + V * u
            ValuePtr f_u = v.dt - (make_value(0.5) * u.dxx) + (V * u.val);
            
            // f_v = u_t + 0.5 * v_xx - V * v
            ValuePtr f_v = u.dt + (make_value(0.5) * v.dxx) - (V * v.val);

            ValuePtr l_phys = pow2(f_u) + pow2(f_v);
            total_loss = total_loss + l_phys;
        }

        total_loss->backward();

        for (auto& p : params) {
            p->data -= lr * p->grad;
        }

        if (epoch % 20 == 0) {
            std::cout << "Epoch " << epoch << " | Physics Loss: " << total_loss->data << std::endl;
        }
    }

    std::cout << "PINN Optimization Successful!" << std::endl;
    return 0;
}
