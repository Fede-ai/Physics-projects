#include <math.h>
#include <SFML/Graphics.hpp>
#pragma once

//https://en.wikipedia.org/wiki/NACA_airfoil
struct NACA {
	NACA(unsigned short code) {
		if (code > 9999) {
			throw std::invalid_argument("NACA code must be a 4-digit number");
		}

		chamber = code / 1000;
		chamberPos = (code / 100) % 10;
		thickness = code % 100;
	}
	unsigned short chamber = 0;
	unsigned short chamberPos = 0;
	unsigned short thickness = 0;

	static double xFromPercent(double percent) {
		return std::pow(percent, 2);
	};

	std::vector<double> getMeanCamberLine(size_t points) const;
	std::vector<double> getCamberSlope(size_t points) const;
	std::vector<double> getThicknessDistribution(size_t points) const;
};

class Foil {
public:
	Foil(NACA inNaca, size_t points)
		:
		naca(inNaca),
		camberLine(sf::PrimitiveType::Lines, points),
		upperFoil(sf::PrimitiveType::LineStrip, points),
		lowerFoil(sf::PrimitiveType::LineStrip, points),
		lines(sf::PrimitiveType::Lines)
	{
		yCamber = naca.getMeanCamberLine(points);
		camberSlope = naca.getCamberSlope(points);
		yThickness = naca.getThicknessDistribution(points);
		buildFoil();
	}
	
	//return angle of attack in radiants
	inline double getAngleOfAttack() const {
		return angleOfAttack;
	}
	//set angle of attack in radiants
	inline void setAngleOfAttack(double angle) {
		angleOfAttack = angle;
		buildFoil();
	}

	sf::VertexArray camberLine;
	sf::VertexArray upperFoil;
	sf::VertexArray lowerFoil;
	sf::VertexArray lines;

	std::vector<double> yCamber;
	std::vector<double> camberSlope;
	std::vector<double> yThickness;

private:
	void buildFoil();

	//in radiants
	double angleOfAttack = 0;

	NACA naca;
};