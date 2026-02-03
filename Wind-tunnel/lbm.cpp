#include "lbm.hpp"

LBM::LBM()
{
	is_solid.assign(NX * NY, 0);
    rho.assign(NX * NY, 1.0);
    ux.assign(NX * NY, 0.0);
    uy.assign(NX * NY, 0.0);
    f.assign(NX * NY * Q, 0.0);
    ftmp.assign(NX * NY * Q, 0.0);

	for (int x = 0; x < NX; x++) {
        is_solid[x] = 1;
   	    is_solid[x + (NY - 1) * NX] = 1;
    }

    // initial equilibrium
    for (int y = 0; y < NY; y++) {
        for (int x = 0; x < NX; x++) {
	        // slightly pre-bias inlet cell
            double ux0 = (x == 0) ? u_in : 0.0;
	        // ux0 = u_in
            for (int k = 0; k < Q; k++)
                f[fIndex(x, y, k)] = feq(k, 1.0, ux0, 0.0);
        }
    }
}

void LBM::applyInletZouHe()
{
	int x = 0;
    for (int y = 1; y < NY - 1; y++) {
        if (is_solid[x + y * NX]) 
	        continue;
        // known populations: f0,f2,f4,f3,f6,f7 known after streaming typically;
        // Here we reconstruct missing ones (1,5,8) for velocity ux = u_in, uy = 0.
        // Simplified Zou/He approach:
        double u0 = u_in;
        // compute rho from known populations:
        double f0 = f[fIndex(x, y, 0)];
        double f2 = f[fIndex(x, y, 2)];
        double f4 = f[fIndex(x, y, 4)];
        double f3 = f[fIndex(x, y, 3)];
        double f6 = f[fIndex(x, y, 6)];
        double f7 = f[fIndex(x, y, 7)];
        double rho_local = (f0 + f2 + f4 + 2.0 * (f3 + f6 + f7)) / (1.0 - u0);
        // set missing populations
        f[fIndex(x,y,1)] = f[fIndex(x,y,3)] + (2.0/3.0)*rho_local*u0;
        f[fIndex(x,y,5)] = f[fIndex(x,y,7)] + 0.5*(f[fIndex(x,y,4)] - f[fIndex(x,y,2)]) + (1.0/6.0)*rho_local*u0;
        f[fIndex(x,y,8)] = f[fIndex(x,y,6)] + 0.5*(f[fIndex(x,y,2)] - f[fIndex(x,y,4)]) + (1.0/6.0)*rho_local*u0;
    }
}

void LBM::applyOutletSimple()
{
	for (int y = 1; y < NY - 1; y++) {
        if (is_solid[NX - 1 + y * NX])
			continue;

		// crude zero-gradient
        for (int k = 0; k < Q; k++)
            f[fIndex(NX - 1, y, k)] = f[fIndex(NX - 2, y, k)]; 
    }
}

void LBM::step()
{
	//collision (write post-collision into ftmp)
    #pragma omp parallel for
    for (int y = 0; y < NY; y++) {
        for (int x = 0; x < NX; x++) {
            int id = x + y * NX;
            int base = fIndex(x, y, 0);

            if (is_solid[id]) {
                //keep distributions unchanged for solids (will be handled by bounce-back after streaming)
                for (int k = 0; k < Q; k++) 
	        	    ftmp[base + k] = f[base + k];
                rho[id] = 1.0;
                ux[id] = 0.0;
                uy[id] = 0.0;
                continue;
            }

            //compute macroscopic from current f
            double rho0 = 0.0, ux0 = 0.0, uy0 = 0.0;
            for (int k = 0; k < Q; k++) {
                double fv = f[base + k];
                rho0 += fv;
                ux0 += fv * ex[k];
                uy0 += fv * ey[k];
            }
            //avoid divide-by-zero (shouldn't happen in well-posed sim)
            if (rho0 <= 0.0) 
	            rho0 = 1e-12;

            ux0 /= rho0;
            uy0 /= rho0;

            rho[id] = rho0;
            ux[id] = ux0;
            uy[id] = uy0;

            //relaxation to equilibrium into ftmp
            for (int k = 0; k < Q; k++) {
                double feqk = feq(k, rho0, ux0, uy0);
                ftmp[base + k] = f[base + k] - (f[base + k] - feqk) / tau;
            }
        }
    }

    //pull streaming (thread-safe)
    //each destination cell reads from its upstream neighbor in ftmp.
    #pragma omp parallel for collapse(2)
    for (int y = 0; y < NY; ++y) {
        for (int x = 0; x < NX; ++x) {
            int base = fIndex(x, y, 0);
            for (int k = 0; k < Q; k++) {
                int xs = x - ex[k];
                int ys = y - ey[k];

                //source outside domain: fallback -> keep local post-collision (conservative)
                //(better: implement explicit BC for edges for mass/velocity control)
                if (xs < 0 || xs >= NX || ys < 0 || ys >= NY)
                    f[base + k] = ftmp[base + k];
                else
                    f[base + k] = ftmp[(ys * NX + xs) * Q + k];
            }
        }
    }

	//compute hydrodynamic force on solids via momentum exchange
	#pragma omp parallel
	{
		double Fx_loc = 0.0, Fy_loc = 0.0;
		#pragma omp for collapse(2) nowait
		for (int y = 0; y < NY; ++y) {
			for (int x = 0; x < NX; ++x) {
				int id = x + y * NX;
				if (!is_solid[id]) 
					continue;

				// for each direction k such that neighbor is fluid (fluid->solid link)
				for (int k = 0; k < Q; ++k) {
					int xf = x + ex[k];
					int yf = y + ey[k];
					if (xf < 0 || xf >= NX || yf < 0 || yf >= NY) 
						continue;
					int idf = xf + yf * NX;
					if (is_solid[idf]) 
						continue; // neighbor also solid -> skip

					// f that arrived at the solid node from the fluid (post-streaming)
					// direction k points from solid to fluid; but the population that moved
					// from fluid node into solid node is f[base_of_s + k] after streaming,
					// or equivalently f[Findex(xf,yf,k)] before streaming? With your pull-stream,
					// we already have `f` = post-streaming values, so:
					double f_in = f[fIndex(x, y, k)]; // this is what sits at solid cell in dir k (post-stream)
					// momentum change: 2 * f_in * e_k
					Fx_loc += 2.0 * f_in * ex[k];
					Fy_loc += 2.0 * f_in * ey[k];
				}
			}
		}
		#pragma omp atomic
		Fx += Fx_loc;
		#pragma omp atomic
		Fy += Fy_loc;
	}

    //bounce-back for solids (half-way bounce-back)
    #pragma omp parallel for 
    for (int y = 0; y < NY; y++) {
        for (int x = 0; x < NX; x++) {
            if (!is_solid[x + y * NX]) 
	            continue;

            int base = fIndex(x, y, 0);
            double tmpQ[Q];
            //read opposite direction (after streaming)
            for (int k = 0; k < Q; k++)
                tmpQ[k] = f[base + opp[k]];
            for (int k = 0; k < Q; k++)
                f[base + k] = tmpQ[k];
        }
    }

    //apply inlet/outlet BCs (they modify f directly)
    applyInletZouHe();
    applyOutletSimple();

    //recompute macroscopic fields after streaming & BCs
    #pragma omp parallel for
    for (int y = 0; y < NY; y++) {
        for (int x = 0; x < NX; x++) {
            int id = x + y * NX;
            double r = 0.0, ux0 = 0.0, uy0 = 0.0;
            for (int k = 0; k < Q; k++) {
                double fv = f[fIndex(x, y, k)];
                r += fv;
                ux0 += fv * ex[k];
                uy0 += fv * ey[k];
            }
            rho[id] = r;

            if (is_solid[id] || r <= 0.0) { 
	        	ux[id] = 0.0; 
	        	uy[id] = 0.0; 
	        }
	        else {
                ux[id] = ux0 / r;
                uy[id] = uy0 / r;
	        }
        }
    }
}
