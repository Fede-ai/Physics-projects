#pragma once
#include <vector>
#include <complex>
#include <cmath>

#define PI 3.141592653589793238462643383279502884L

typedef std::vector<std::complex<double>> List;

class Transform {
public:
	Transform() = default;
	void performDFT(const List& x);
	List performIDFT();

	List spectrum_;

private:
	
};