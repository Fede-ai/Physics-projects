#include "gravity.hpp"
#include <cmath>
#include <algorithm>

double GravitySimulator::getPotentialAtPoint(double x, double z) const
{
	double potential = 0.0;
	for (const auto& b : bodies_) {
		//skip massless bodies
		if (b->mass <= 0)
			continue;

		double dx = x - b->position.x;
		double dz = z - b->position.z;
		double r = std::sqrt(dx * dx + dz * dz);

		//potential outside the mass
		if (r >= b->radius)
			potential += G * b->mass / r;
		//potential inside the mass
		else {
			double R = b->radius;
			double rr = r / R;
			potential += G * b->mass * (3.0 - rr * rr) / (2.0 * R);
		}
	}

	return potential * potentialScaling;
}

void GravitySimulator::step(double dt)
{
	const size_t n = bodies_.size();
	std::vector<Vec3> accels;

	computeAccelerations(accels);
	for (size_t i = 0; i < n; ++i)
		bodies_[i]->velocity += accels[i] * (0.5 * dt);

	for (auto* b : bodies_)
		b->position += b->velocity * dt;

	computeAccelerations(accels);
	for (size_t i = 0; i < n; ++i)
		bodies_[i]->velocity += accels[i] * (0.5 * dt);
}

void GravitySimulator::computeAccelerations(std::vector<Vec3>& accels)
{
	constexpr double eps2 = 1e-12;
	const size_t n = bodies_.size();
	accels.assign(n, Vec3{ 0.0, 0.0, 0.0 });

	for (size_t i = 0; i < n; i++) {
		for (size_t j = i + 1; j < n; j++) {
			if (bodies_[j]->mass == 0.0 && bodies_[i]->mass == 0.0)
				continue;

			Vec3 r = bodies_[j]->position - bodies_[i]->position;
			double dist2 = r.dot(r) + eps2;
			double invDist = 1.0 / std::sqrt(dist2);
			double invDist3 = invDist * invDist * invDist;

			Vec3 forceDir = r * (G * invDist3);
			accels[i] += forceDir * bodies_[j]->mass;
			accels[j] -= forceDir * bodies_[i]->mass;
		}
	}
}
