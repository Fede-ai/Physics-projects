#include "pendulum.hpp"
#include <cmath>

Pendulum::Pendulum(double inM1, double inM2, double inL1, double inL2)
	:
	m1(inM1),
	m2(inM2),
	l1(inL1),
	l2(inL2)
{
}

void Pendulum::step(double dt)
{
    auto deriv = [&](const State& st) {
        State d;
        auto acc = computeAccelerations(st);
        d.theta1 = st.omega1;
        d.theta2 = st.omega2;
        d.omega1 = acc.first;
        d.omega2 = acc.second;
        return d;
        };

    State k1 = deriv(state);

    State state2 {
        state.theta1 + 0.5 * dt * k1.theta1,
        state.theta2 + 0.5 * dt * k1.theta2,
        state.omega1 + 0.5 * dt * k1.omega1,
        state.omega2 + 0.5 * dt * k1.omega2
    };
    State k2 = deriv(state2);

    State state3 {
        state.theta1 + 0.5 * dt * k2.theta1,
        state.theta2 + 0.5 * dt * k2.theta2,
        state.omega1 + 0.5 * dt * k2.omega1,
        state.omega2 + 0.5 * dt * k2.omega2
    };
    State k3 = deriv(state3);

    State state4 {
        state.theta1 + dt * k3.theta1,
        state.theta2 + dt * k3.theta2,
        state.omega1 + dt * k3.omega1,
        state.omega2 + dt * k3.omega2
    };
    State k4 = deriv(state4);

    state = {
        state.theta1 + dt / 6.0 * (k1.theta1 + 2 * k2.theta1 + 2 * k3.theta1 + k4.theta1),
        state.theta2 + dt / 6.0 * (k1.theta2 + 2 * k2.theta2 + 2 * k3.theta2 + k4.theta2),
        state.omega1 + dt / 6.0 * (k1.omega1 + 2 * k2.omega1 + 2 * k3.omega1 + k4.omega1),
        state.omega2 + dt / 6.0 * (k1.omega2 + 2 * k2.omega2 + 2 * k3.omega2 + k4.omega2)
    };
}

double Pendulum::getKineticEnergy() const
{
    double KE1 = 0.5 * m1 * (l1 * l1) * (state.omega1 * state.omega1);
    double KE2 = 0.5 * m2 * (
        (l1 * l1) * (state.omega1 * state.omega1) +
        (l2 * l2) * (state.omega2 * state.omega2) +
        2 * l1 * l2 * state.omega1 * state.omega2 * cos(state.theta1 - state.theta2)
    );
	return KE1 + KE2;
}

double Pendulum::getPotentialEnergy() const
{
	double PE1 = m1 * g * l1 * (1 - cos(state.theta1));
	double PE2 = m2 * g * (l1 + l2 - (l1 * cos(state.theta1) + l2 * cos(state.theta2)));
	return PE1 + PE2;
}

std::pair<double, double> Pendulum::computeAccelerations(const State& s) const
{
	std::pair<double, double> acc;
    double dtheta = s.theta2 - s.theta1;

    double denom1 = (m1 + m2) * l1 - m2 * l1 * cos(dtheta) * cos(dtheta);
    double denom2 = (l2 / l1) * denom1;

    acc.first = (m2 * l1 * s.omega1 * s.omega1 * sin(dtheta) * cos(dtheta)
            + m2 * g * sin(s.theta2) * cos(dtheta)
            + m2 * l2 * s.omega2 * s.omega2 * sin(dtheta)
            - (m1 + m2) * g * sin(s.theta1)) / denom1;

    acc.second = (-m2 * l2 * s.omega2 * s.omega2 * sin(dtheta) * cos(dtheta)
            + (m1 + m2) * g * sin(s.theta1) * cos(dtheta)
            - (m1 + m2) * l1 * s.omega1 * s.omega1 * sin(dtheta)
            - (m1 + m2) * g * sin(s.theta2)) / denom2;

    return acc;
}
