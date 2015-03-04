/**
* \file BHE_2U.h
* 2014/06/04 HS inital implementation
* class of borehole heat exchanger with 2 U-tube
*
*/

#ifndef BHE_2U_H
#define BHE_2U_H

#include "BHEAbstract.h"
#include "makros.h"

namespace BHE  // namespace of borehole heat exchanger
{

	class BHE_2U : public BHEAbstract
	{
	public:
		/**
		  * constructor
		  */
        BHE_2U(const std::string name               /* name of the BHE */,
               BHE::BHE_BOUNDARY_TYPE bound_type    /* type of BHE boundary */,
               double my_L          = 100           /* length/depth of the BHE */,
			   double my_D          = 0.013         /* diameter of the BHE */, 
			   double my_Qr         = 21.86 / 86400 /* total refrigerant flow discharge of BHE */,
			   double my_r_inner    = 0.016         /* inner radius of the pipline */,
			   double my_r_outer    = 0.016         /* outer radius of the pipline */,
			   double my_b_in       = 0.0029        /* pipe-in wall thickness*/,
			   double my_b_out      = 0.0029        /* pipe-out wall thickness*/,
			   double my_mu_r       = 0.00054741    /* dynamic viscosity of the refrigerant */,
			   double my_rho_r      = 988.1         /* density of the refrigerant */,
			   double my_alpha_L    = 1.0e-4         /* longitudinal dispersivity of the refrigerant in the pipeline */,
               double my_heat_cap_r = 4180          /* specific heat capacity of the refrigerant */,
               double my_rho_g      = 2190          /* density of the grout */,
			   double my_porosity_g = 0.5           /* porosity of the grout */,
               double my_heat_cap_g = 1000          /* specific heat capacity of the grout */,
			   double my_lambda_r   = 0.6405        /* thermal conductivity of the refrigerant */,
			   double my_lambda_p   = 0.38          /* thermal conductivity of the pipe wall */,
			   double my_lambda_g   = 2.3           /* thermal conductivity of the grout */, 
			   double my_omega      = 0.04242       /* pipe distance */,
               double my_power_in_watt = 0.0      /* injected or extracted power */,
			   BHE_DISCHARGE_TYPE type = BHE::BHE_DISCHARGE_TYPE_PARALLEL) 
               : BHEAbstract(BHE::BHE_TYPE_2U, name, bound_type),
			_discharge_type(type)
		{
			_u = Eigen::Vector4d::Zero();
			_Nu = Eigen::Vector4d::Zero(); 

			L = my_L;
			D = my_D; 
			Q_r = my_Qr;
			r_inner = my_r_inner;
			r_outer = my_r_outer; 
			b_in = my_b_in; 
			b_out = my_b_out; 
			mu_r = my_mu_r; 
			rho_r = my_rho_r; 
			alpha_L = my_alpha_L;
			heat_cap_r = my_heat_cap_r; 
            rho_g = my_rho_g; 
            heat_cap_g = my_heat_cap_g; 
			porosity_g = my_porosity_g;
			lambda_r = my_lambda_r;
			lambda_p = my_lambda_p; 
			lambda_g = my_lambda_g; 
			omega = my_omega; 
            power_in_watt_val = my_power_in_watt;

			S_i  = PI * 2.0 * r_outer;
			S_o  = PI * 2.0 * r_outer; 
			S_g1 = 0.5 * D; 
			S_g2 = D;
			S_gs = 0.25 * PI * D; 

            // cross section area calculation
            CSA_i = PI * r_inner * r_inner;
            CSA_o = PI * r_inner * r_inner;
            CSA_g1 = 0.125 * PI * D * D - CSA_i;
            CSA_g2 = 0.125 * PI * D * D - CSA_o;

			// initialization calculation
			initialize(); 
		}; 

		/**
		  * return the number of unknowns needed for 2U BHE
		  */
		std::size_t get_n_unknowns() { return 8; }

        /**
          * return the number of boundary heat exchange terms for this BHE
          * abstract function, need to be realized.
          */
        std::size_t get_n_heat_exchange_terms() { return 5; }

		/**
		  * return the thermal resistance for the inlet pipline
		  * idx is the index, when 2U case,
		  * 0 - the first u-tube
		  * 1 - the second u-tube
		  */
		double get_thermal_resistance_fig(std::size_t idx);

		/**
		  * return the thermal resistance for the outlet pipline
		  * idx is the index, when 2U case,
		  * 0 - the first u-tube
		  * 1 - the second u-tube
		  */
		double get_thermal_resistance_fog(std::size_t idx);

		/**
          * return the thermal resistance
		  */
		double get_thermal_resistance(std::size_t idx);

		/**
		  * calculate thermal resistance
		  */
		void calc_thermal_resistances();
		
		/**
		  * Nusselt number calculation
		  */
		void calc_Nu();

		/**
		  * Renolds number calculation
		  */
		void calc_Re();

		/**
		  * Prandtl number calculation
		  */
		void calc_Pr();

		/**
		  * flow velocity inside the pipeline
		  */
		void calc_u();

		/**
		  * calculate heat transfer coefficient
		  */
		void calc_heat_transfer_coefficients();

        /**
          * return the coeff of mass matrix,
          * depending on the index of unknown.
          */
        double get_mass_coeff(std::size_t idx_unknown);

        /**
          * return the coeff of laplace matrix,
          * depending on the index of unknown.
          */
        void get_laplace_matrix(std::size_t idx_unknown, Eigen::MatrixXd & mat_laplace);

        /**
          * return the coeff of advection matrix,
          * depending on the index of unknown.
          */
        void get_advection_vector(std::size_t idx_unknown, Eigen::VectorXd & vec_advection);

        /**
          * return the coeff of boundary heat exchange matrix,
          * depending on the index of unknown.
          */
        double get_boundary_heat_exchange_coeff(std::size_t idx_unknown);

        /**
          * return the shift index based on primary variable value
          */
        int get_loc_shift_by_pv(FiniteElement::PrimaryVariable pv_name);

        /**
          * return the number of grout zones in this BHE.
          */
        std::size_t get_n_grout_zones(void) { return 4; };

        /**
          * return the inflow temperature based on outflow temperature and fixed power.
          */
        double get_Tin_by_Tout_and_power(double T_out);

        /**
          * required by eigen library, 
          * to make sure the dynamically allocated class has 
          * aligned "operator new"
          */
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		
	private:
		/**
		  * thermal resistances 
		  */
		double _R_fig, _R_fog; 

		/**
		  * thermal resistances due to advective flow of refrigerant in the pipes
		  */
		double _R_adv_i1, _R_adv_i2, _R_adv_o1, _R_adv_o2;

		/**
          * thermal resistances due to the pipe wall material 
		  */
		double _R_con_a_i1, _R_con_a_i2, _R_con_a_o1, _R_con_a_o2;

		/**
		  * thermal resistances due to the grout transition
		  */
		double _R_con_b; 

		/**
		  * thermal resistances of the grout
		  */
		double _R_g;

		/**
		  * thermal resistances of the grout soil exchange
		  */
		double _R_gs; 

		/**
		  * thermal resistances due to inter-grout exchange
		  */
		double _R_gg_1, _R_gg_2;

		/**
		  * heat transfer coefficients
		  */
		double _PHI_fig, _PHI_fog, _PHI_gg_1, _PHI_gg_2, _PHI_gs;

		/**
		  * Reynolds number
		  */
		double _Re;

		/**
		  * Prandtl number
		  */
		double _Pr;

		/**
		  * Nusselt number
		  */
		Eigen::Vector4d _Nu;

		/**
		  * flow velocity inside the pipeline
		  */
		Eigen::Vector4d _u;

		/**
		  * discharge type of the 2U BHE
		  */
		const BHE_DISCHARGE_TYPE _discharge_type; 

		/**
          * pipe distance
		  */
		double omega; 

		/**
		  * specific exchange surfaces S
		  */
		double S_i, S_o, S_g1, S_g2, S_gs;
        /**
          * cross section area
          */
        double CSA_i, CSA_o, CSA_g1, CSA_g2;
	};


}  // end of namespace

#endif
