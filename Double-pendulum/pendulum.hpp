#pragma once
#include <utility>

class Pendulum {
public:		
	struct State {
		double theta1 = 0;
		double theta2 = 0;
		double omega1 = 0;
		double omega2 = 0;
	};

	Pendulum(double inM1, double inM2, double inL1, double inL2);
	void step(double dt);
	const State& setState(State s) { state = s; return state; }

	double getKineticEnergy() const;
	double getPotentialEnergy() const;
	std::pair<double, double> getArmLengths() const { return { l1, l2 }; }

private:
	std::pair<double, double> computeAccelerations(const State& s) const;
	
	State state;
	const double m1, m2;
	const double l1, l2;
	const double g = 9.80665;
};