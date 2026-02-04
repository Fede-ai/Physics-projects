#include "gravity.hpp"
#include <cmath>
#include <algorithm>

double GravitySimulator::getPotentialAtPoint(double x, double z)
{
	double dx = 0.01 * x;
	double dz = 0.01 * z;
	double r = sqrt(dx * dx + dz * dz);
	return std::min(10 / std::max(r, 0.001), 500.0);
}
