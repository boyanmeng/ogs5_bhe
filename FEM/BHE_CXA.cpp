/**
* \file BHE_CXA.cpp
* 2014/06/04 HS inital implementation
* class of borehole heat exchanger with coaxial annular pipeline
*
*/

#include "BHE_CXA.h"
#include "makros.h"


using namespace BHE;
/**
* return the thermal resistance for the inlet pipline
* idx is the index, when 2U case,
* 0 - the first u-tube
* 1 - the second u-tube
*/
double BHE_CXA::get_thermal_resistance_fig(std::size_t idx = 0)
{
	// TODO
	return 0.0;
}

/**
* return the thermal resistance for the outlet pipline
* idx is the index, when 2U case,
* 0 - the first u-tube
* 1 - the second u-tube
*/
double BHE_CXA::get_thermal_resistance_fog(std::size_t idx = 0)
{
	// TODO
	return 0.0;
}

/**
* return the thermal resistance
*/
double BHE_CXA::get_thermal_resistance(std::size_t idx = 0)
{
	// TODO
	return 0.0;
}

/**
* calculate thermal resistance
*/
void BHE_CXA::calc_thermal_resistances()
{
	double Nu_in, Nu_out; 
	double d_o1, d_i1, d_h;
	double chi;
	double _R_con_i1, _R_con_o1;

	Nu_in = _Nu(0);
	Nu_out = _Nu(1);
	d_o1 = 2.0 * r_inner;
	d_i1 = 2.0 * r_outer;
	d_h = 2.0 * r_outer - d_o1;

	// thermal resistance due to advective flow of refrigerant in the pipes
	// Eq. 58, 59, and 60 in Diersch_2011_CG
	_R_adv_o1 = 1.0 / (Nu_out * lambda_r * PI);
	_R_adv_a_i1 = 1.0 / (Nu_in * lambda_r * PI) * ( d_h / d_o1) ;
	_R_adv_b_i1 = 1.0 / (Nu_in * lambda_r * PI) * ( d_h / d_i1);

	// thermal resistance due to thermal conductivity of the pip wall material
	// Eq. 66 in Diersch_2011_CG
	_R_con_i1 = std::log( (r_outer + b_out) / r_outer) / (2.0 * PI * lambda_p);
	_R_con_o1 = std::log( (r_inner + b_out) / r_inner) / (2.0 * PI * lambda_p);

	// thermal resistance due to the grout transition
	d_i1 = 2.0 * r_outer;
	// Eq. 68
	chi = std::log(std::sqrt(D*D + d_i1*d_i1) / std::sqrt(2) / d_i1) / std::log(D / d_i1);
	// Eq. 69
	_R_g = std::log(D/d_i1) / (2.0 * PI * lambda_g);
	// Eq. 67
	_R_con_b = chi * _R_g;
	// Eq. 56 and 57
	_R_ff = _R_adv_o1 + _R_adv_a_i1 + _R_con_o1;
	_R_fig = _R_adv_b_i1 + _R_con_i1 + _R_con_b;

	// thermal resistance due to grout-soil exchange
	_R_gs = (1 - chi)*_R_g;

}

/**
* Nusselt number calculation
*/
void BHE_CXA::calc_Nu()
{
	// see Eq. 32 in Diersch_2011_CG

	double Nu_in(0.0), Nu_out(0.0);
	double gamma, xi;
	double d_o1, d_i1, d_h;

	d_o1 = 2.0 * r_inner;
	d_i1 = 2.0 * r_outer;
	d_h = 2.0 * r_outer - d_o1;

	// first calculating Nu_out
	if (_Re_o1 < 2300.0)
	{
		Nu_out = 4.364;
	}
	else if (_Re_o1 >= 2300.0 && _Re_o1 < 10000.0)
	{
		gamma = (_Re_o1 - 2300) / (10000 - 2300);

		Nu_out = (1.0 - gamma) * 4.364;
		Nu_out += gamma * ((0.0308 / 8.0 * 1.0e4 * _Pr) / (1.0 + 12.7 * std::sqrt(0.0308 / 8.0) * (std::pow(_Pr, 2.0 / 3.0) - 1.0)) * (1.0 + std::pow(d_o1 / L, 2.0 / 3.0)));

	}
	else if (_Re_o1 > 10000.0)
	{
		xi = pow(1.8 * std::log10(_Re_o1) - 1.5, -2.0);
		Nu_out = (xi / 8.0 * _Re_o1 * _Pr) / (1.0 + 12.7 * std::sqrt(xi / 8.0) * (std::pow(_Pr, 2.0 / 3.0) - 1.0)) * (1.0 + std::pow(d_o1 / L, 2.0 / 3.0));
	}

	// then calculating Nu_in
	if (_Re_i1 < 2300.0)
	{
		Nu_in = 3.66;
		Nu_in += (4.0 - 0.102 / (d_o1 / d_i1 + 0.02)) * pow(d_o1 / d_i1, 0.04);
	}
	else if (_Re_i1 >= 2300.0 && _Re_i1 < 10000.0)
	{
		gamma = (_Re_i1 - 2300) / (10000 - 2300);

		Nu_in = (1.0 - gamma) * (3.66 + (4.0 - 0.102 / (d_o1 / d_i1 + 0.02))) * pow(d_o1 / d_i1, 0.04);
		Nu_in += gamma * ((0.0308 / 8.0 * 1.0e4 * _Pr) / (1.0 + 12.7 * std::sqrt(0.0308 / 8.0) * (std::pow(_Pr, 2.0 / 3.0) - 1.0)) * (1.0 + std::pow(d_h / L, 2.0 / 3.0)) * ((0.86 * std::pow(d_o1 / d_i1, 0.84) + 1.0 - 0.14*std::pow(d_o1 / d_i1, 0.6)) / (1.0 + d_o1 / d_i1)));

	}
	else if (_Re_i1 > 10000.0)
	{
		xi = pow(1.8 * std::log10(_Re_i1) - 1.5, -2.0);
		Nu_in = (xi / 8.0 * _Re_i1 * _Pr) / (1.0 + 12.7 * std::sqrt(xi / 8.0) * (std::pow(_Pr, 2.0 / 3.0) - 1.0)) * (1.0 + std::pow(d_h / L, 2.0 / 3.0)) * ((0.86 * std::pow(d_o1 / d_i1, 0.84) + 1.0 - 0.14*std::pow(d_o1 / d_i1, 0.6)) / (1.0 + d_o1 / d_i1));
	}

	// _Nu(0) is Nu_in, and _Nu(1) is Nu_out
	_Nu(0) = Nu_in;
	_Nu(1) = Nu_out;
}

/**
* Renolds number calculation
*/
void BHE_CXA::calc_Re()
{
	double d_o1, d_h;

	d_o1 = 2.0 * r_inner; // inner diameter of the pipeline
	d_h = 2.0 * r_outer - d_o1; 

	// _u(0) is u_in, and _u(1) is u_out
	_Re_o1 = _u(1) * d_o1 / (mu_r / rho_r);
	_Re_i1 = _u(0) * d_h  / (mu_r / rho_r);
}

/**
* Prandtl number calculation
*/
void BHE_CXA::calc_Pr()
{
	_Pr = mu_r * heat_cap_r / lambda_r;
}

/**
* calculate heat transfer coefficient
*/
void BHE_CXA::calc_heat_transfer_coefficients()
{
	_PHI_fig = 1.0 / _R_fig * 1.0 / S_i;
	_PHI_ff  = 1.0 / _R_ff * 1.0 / S_io;
	_PHI_gs = 1.0 / _R_gs * 1.0 / S_gs;
}

/**
* flow velocity inside the pipeline
*/
void BHE_CXA::calc_u()
{
	double u_in, u_out;

	u_in = Q_r / (2.0 * PI * (r_outer * r_outer - r_inner * r_inner));
	u_out = Q_r / (2.0 * PI * r_inner * r_inner);

	_u(0) = u_in;
	_u(1) = u_out;
}

double BHE_CXA::get_mass_coeff(std::size_t idx_unknown)
{
    double mass_coeff = 0.0;

    switch (idx_unknown)
    {
    case 0:  // i1
        mass_coeff = rho_r * heat_cap_r;
        break;
    case 1:  // i2
        mass_coeff = rho_r * heat_cap_r;
        break;
    case 2:  // o1
        mass_coeff = rho_g * heat_cap_g;
        break;
    default:
        break;
    }

    return mass_coeff;
}

double BHE_CXA::get_laplace_coeff(std::size_t idx_unknown)
{
	// Here we calculates the laplace coefficients in the governing 
	// equations of BHE. These governing equations can be found in 
	// 1) Diersch (2013) FEFLOW book on page 952, M.120-122, or
	// 2) Diersch (2011) Comp & Geosci 37:1122-1135, Eq. 23-25. 
	double laplace_coeff(0.0);

	switch (idx_unknown)
	{
	case 0:
		// pipe i1, Eq. 23
		laplace_coeff = lambda_r + rho_r * heat_cap_r * _u.norm();
		break;
	case 1:
		// pipe o1, Eq. 24
		laplace_coeff = lambda_r + rho_r * heat_cap_r * _u.norm();
		break;
	case 2:
		// pipe g1, Eq. 25
		laplace_coeff = porosity_g * lambda_g;
		break;
	default:
		std::cout << "Error !!! The index passed to get_laplace_coeff for BHE is not correct. \n";
		exit(1);
		break;
	}
	return laplace_coeff;
}

double BHE_CXA::get_advection_coeff(std::size_t idx_unknown)
{
    double advection_coeff(0);
    // TODO
    return advection_coeff;
}