#include "gravity.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

GravitySimulator::GravitySimulator(unsigned int fieldSideSize, unsigned int candidatesPerSide)
	:
	fieldSideSize_(fieldSideSize),
	candidatesPerSide_(candidatesPerSide)
{
}

double GravitySimulator::getPotentialAtPoint(double x, double z) const
{
	double potential = 0.0;
	for (const auto& b : bodies_) {
		//skip massless bodies
		if (b->mass <= 0.0)
			continue;

		double dx = x - b->position.x;
		double dz = z - b->position.z;
		double r = std::sqrt(dx * dx + dz * dz);

		//outside the mass
		if (r >= b->radius)
			potential += -G * b->mass / r;
		//inside the mass (uniform sphere)
		else {
			double R = b->radius;
			double rr = r / R;
			potential += -G * b->mass * (3.0 - rr * rr) / (2.0 * R);
		}
	}

	return potential * potentialScaling;
}

Gradient GravitySimulator::getGradientAtPoint(double x, double z) const
{
    Gradient g;
	for (const auto& b : bodies_) {
		//skip massless bodies
		if (b->mass <= 0.0)
			continue;

		double dx = x - b->position.x;
		double dz = z - b->position.z;
		double r = std::sqrt(dx * dx + dz * dz);

		//not a shortcut, actual formula
		if (r < b->radius)
			r = b->radius;

		double invr3 = 1.0 / (r * r * r);
		g.x += G * b->mass * dx * invr3;
		g.z += G * b->mass * dz * invr3;
	}

	return g;
}

Hessian GravitySimulator::getHessianAtPoint(double x, double z) const
{
    constexpr double h = 1e-4;

    auto g0 = getGradientAtPoint(x, z);
    auto gx1 = getGradientAtPoint(x + h, z);
    auto gx2 = getGradientAtPoint(x - h, z);
    auto gz1 = getGradientAtPoint(x, z + h);
    auto gz2 = getGradientAtPoint(x, z - h);

    Hessian H;
    H.xx = (gx1.x - gx2.x) / (2.0 * h);
    H.xz = (gz1.x - gz2.x) / (2.0 * h);
    H.zx = (gx1.z - gx2.z) / (2.0 * h);
    H.zz = (gz1.z - gz2.z) / (2.0 * h);

    return H;
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

void GravitySimulator::calculateStabilityPoints(std::vector<Vec3>& points) const
{
    points.clear();

    const double d = fieldSideSize_ / double(candidatesPerSide_);
    constexpr int maxNewtonIter = 50;
    constexpr double gradThreshold = 1e-8;
    constexpr double stepDamping = 0.5;

	//check if the sign doesnt agree
    auto sc = [](double a, double b) {
        return (a <= 0.0 && b >= 0.0) || (a >= 0.0 && b <= 0.0);
        };

    std::vector<Vec3> candidates;
    for (int i = 0; i < candidatesPerSide_; ++i) {
        for (int j = 0; j < candidatesPerSide_; ++j) {
            double x0 = -fieldSideSize_ / 2.f + i * d;
            double z0 = -fieldSideSize_ / 2.f + j * d;

            auto g00 = getGradientAtPoint(x0, z0);
            auto g10 = getGradientAtPoint(x0 + d, z0);
            auto g01 = getGradientAtPoint(x0, z0 + d);
            auto g11 = getGradientAtPoint(x0 + d, z0 + d);

            bool xZero = sc(g00.x, g10.x) || sc(g00.x, g01.x) || sc(g00.x, g11.x);
            bool zZero = sc(g00.z, g10.z) || sc(g00.z, g01.z) || sc(g00.z, g11.z);

            if (!xZero || !zZero)
                continue;

            double x = x0 + 0.5 * d;
            double z = z0 + 0.5 * d;

            for (int iter = 0; iter < maxNewtonIter; ++iter) {
                auto g = getGradientAtPoint(x, z);
                double gnorm = std::hypot(g.x, g.z);

                if (gnorm < gradThreshold)
                    break;

                Hessian H = getHessianAtPoint(x, z);
                double det = H.xx * H.zz - H.xz * H.zx;

                if (std::abs(det) < 1e-12)
                    break;

                //newton step
                double dx = (-H.zz * g.x + H.xz * g.z) / det;
                double dz = (H.zx * g.x - H.xx * g.z) / det;

                x += stepDamping * dx;
                z += stepDamping * dz;
            }

            auto gFinal = getGradientAtPoint(x, z);
            if (std::hypot(gFinal.x, gFinal.z) < gradThreshold)
                candidates.push_back({ x, 0.0, z });
        }
    }

    //deduplication
    constexpr double mergeThreshold = 0.0001;
    for (const auto& c : candidates) {
        bool unique = true;
        for (const auto& p : points) {
            if (std::hypot(c.x - p.x, c.z - p.z) < mergeThreshold) {
                unique = false;
                break;
            }
        }
        if (unique)
            points.push_back(c);
    }

    for (auto& p : points)
        p.y = -getPotentialAtPoint(p.x, p.z);
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
