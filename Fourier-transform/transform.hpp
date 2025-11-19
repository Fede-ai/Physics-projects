#pragma once
#include <vector>
#include <complex>
#include <cmath>

#define PI 3.141592653589793238462643383279502884L

typedef std::complex<double> Point;
typedef struct {
	int m;
	double magnitude;
	double phase;
} Freq;

class Transform {
public:
	Transform(const std::vector<Point>& x);

	static std::vector<Point> smoothenPoints(const std::vector<Point>& x);

	std::vector<Point> spectrum_;
	std::vector<Freq> orderedSpectrum_;

private:
	void orderSpectrum();
	std::vector<Point> performIDFT();
	
};