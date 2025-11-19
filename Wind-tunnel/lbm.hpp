#include <vector>

//lattice parameters for D2Q9
const int Q = 9;
const int ex[Q] = {0, 1, 0, -1, 0, 1, -1, -1, 1};
const int ey[Q] = {0, 0, 1, 0, -1, 1, 1, -1, -1};
const double w[Q] = {4.0/9.0,
                     1.0/9.0,1.0/9.0,1.0/9.0,1.0/9.0,
                     1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0};
const double cs2 = 1.0/3.0; //speed of sound squared
const int opp[Q] = {0, 3, 4, 1, 2, 7, 8, 5, 6};

//simulation parameters
const int NX = 3 * 250, NY = 2 * 250;
const double u_in = 0.05;       //inlet velocity in lattice units
const double nu = 0.02;         //kinematic viscosity (l.u.)
const double tau = 0.5 + nu/cs2;//relaxation time

class LBM {
public:
	LBM();
	std::pair<double, double> performSteps(size_t num) {
		Fx = 0.0, Fy = 0.0;
		for (size_t i = 0; i < num; i++)
			step();
		return {Fx / num, Fy / num};
	}

	//vector of chars and not bools for performance
	std::vector<char> is_solid;
	//size NX * NY
	std::vector<double> rho, ux, uy;

private:
	//helpers for indexing distribution arrays
	inline int fIndex(int x, int y, int i) { 
		return (y * NX + x) * Q + i; 
	}
	//equilibrium
	inline double feq(int i, double rho0, double u0x, double u0y) {
  	double eiu = ex[i] * u0x + ey[i] * u0y;
  	double uu = u0x * u0x + u0y * u0y;
  	return w[i] * rho0 * (1.0 + 3.0 * eiu + 4.5 * eiu * eiu - 1.5 * uu);
	}

	//Zou/He velocity boundary on left side (simple)
	void applyInletZouHe();
	//simple outflow (copy from neighbor)
	void applyOutletSimple();

	void step();

	//size NX * NY * Q
	std::vector<double> f, ftmp;
	double Fx = 0.0, Fy = 0.0;
};
