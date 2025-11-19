#include "foil.hpp"

std::vector<double> NACA::getMeanCamberLine(size_t points) const {
	std::vector<double> line(points, 0.0);
	if (chamber == 0) 
		return line;

	for (size_t i = 0; i < points; i++) {
		double x = xFromPercent(double(i) / (points - 1));
		double m = chamber / 100.0;
		double p = chamberPos / 10.0;

		if (x < p)
			line[i] = (m / (p * p)) * (2 * p * x - x * x);
		else
			line[i] = (m / ((1 - p) * (1 - p))) * ((1 - 2 * p) + 2 * p * x - x * x);
	}
	return line;
}

std::vector<double> NACA::getCamberSlope(size_t points) const {
	std::vector<double> slope(points, 0.0);
	if (chamber == 0) 
		return slope;

	for (size_t i = 0; i < points; i++) {
		double x = xFromPercent(double(i) / (points - 1));
		double m = chamber / 100.0;
		double p = chamberPos / 10.0;

		if (x < p)
			slope[i] = (2 * m / (p * p)) * (p - x);
		else
			slope[i] = (2 * m / ((1 - p) * (1 - p))) * (p - x);
	}

	return slope;
}

std::vector<double> NACA::getThicknessDistribution(size_t points) const {
	std::vector<double> dist(points);
	for (size_t i = 0; i < points; i++) {
		double x = xFromPercent(double(i) / (points - 1));
		dist[i] = 5 * (thickness / 100.0) * (0.2969 * sqrt(x) - 0.1260 * x - 0.3516 * x * x + 0.2843 * x * x * x - 0.1036 * x * x * x * x);
	}
	return dist;
}

void Foil::buildFoil() {
	auto angle = angleOfAttack;
	//lambda function to rotate a point by the angle of attack
	auto rotate = [angle](sf::Vector2f p) {
		double d = sqrt(p.x * p.x + p.y * p.y);
		double a = atan2(p.y, p.x);

		return sf::Vector2f(float(cos(a + angle) * d), float(sin(a + angle) * d));
	};

	lines.clear();
	//construct camber line and upper/lower foils
	for (size_t i = 0; i < yCamber.size(); i++) {
		double x = NACA::xFromPercent(i / double(yCamber.size() - 1)) - 0.5;
		double y = yCamber[i];
		double yt = yThickness[i];

		sf::Vertex v;
		v.color = sf::Color::Black;
		sf::Vector2f p;

		//construct camber line
		p = {float(x), float(-y)};
		v.position = rotate(p);
		camberLine[i] = v;

		//calculate the angle parallel to the camber
		const double a = atan(camberSlope[i]);

		//apply thickness perpendicularly to the camber to construct upper/lower foils
		p = {float(x - yt * sin(a)), float(-y - yt * cos(a))};
		v.position = rotate(p);
		upperFoil[i] = v;
		p = {float(x + yt * sin(a)), float(-y + yt * cos(a))};
		v.position = rotate(p);
		lowerFoil[i] = v;

		//approximate the max thickness line
		if (x > -0.2 && lines.getVertexCount() == 0) {
			double lastX = NACA::xFromPercent((i - 1) / double(yCamber.size() - 1)) - 0.5;
			float wBef = abs(0.2 + x) / (x - lastX);
			float wAft = abs(0.2 + lastX) / (x - lastX);

			float xu, yu, xl, yl;
			xu = upperFoil[i - 1].position.x * wBef + upperFoil[i].position.x * wAft;
			yu = upperFoil[i - 1].position.y * wBef + upperFoil[i].position.y * wAft;
			xl = lowerFoil[i - 1].position.x * wBef + lowerFoil[i].position.x * wAft;
			yl = lowerFoil[i - 1].position.y * wBef + lowerFoil[i].position.y * wAft;

			sf::Vertex v;
			v.color = sf::Color::Red;
			v.position = {xu, yu};
			lines.append(v);
			v.position = {xl, yl};
			lines.append(v);
		}
	}
}