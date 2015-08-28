/**
* \file BHE_Net_ELE_Abstract.h
* 2015/08/27 HS inital implementation
* borehole heat exchanger network element abstract class
*
*
*/

#ifndef BHE_NET_ELE_ABSTRACT_H
#define BHE_NET_ELE_ABSTRACT_H

#include "Eigen/Eigen"

namespace BHE  // namespace of borehole heat exchanger
{
    /**
    * list the types of BHE net element
    */
    enum BHE_NET_ELE {
        BHE_NET_PIPE,          // pipeline
        BHE_NET_DISTRIBUTOR,   // distributor
        BHE_NET_HEATPUMP,      // heat pump
        BHE_NET_BOREHOLE	   // borehole
    };

    class BHE_Net_ELE_Abstract {
    
    public:
        /**
          * constructor
          */
        BHE_Net_ELE_Abstract(BHE_NET_ELE type, int n_inlet = 1, int n_outlet = 1) 
            : N_IN(n_inlet), N_OUT(n_outlet), _ele_type(type)
        {
            int i; 
            // initialize T_in
            T_in = new double[N_IN];
            for (i = 0; i < N_IN; i++)
            {
                T_in[i] = 0.0;
            }

            // initialize T_out
            T_out = new double[N_OUT];
            for (i = 0; i < N_OUT; i++)
            {
                T_out[i] = 0.0;
            }
        }

        /**
          * destructor
          */
        ~BHE_Net_ELE_Abstract(){
            delete [] T_in; 
            delete [] T_out; 
        }

        /**
          * get inlet temperature
          */
        double get_T_in(int idx = 0) {
            return T_in[idx];
        }

        /**
          * get outlet temperature
          */
        double get_T_out(int idx = 0) {
            return T_out[idx];
        }

        /**
          * set inlet temperature
          */
        void set_T_in(double val, int idx = 0){
            T_in[idx] = val;
        }

        /**
          * set outlet temperature
          */
        void set_T_out(double val, int idx = 0){
            T_out[idx] = val; 
        }

        /**
          * return the number of inlet temperatures
          */
        int get_n_T_in()
        {
            return N_IN; 
        }

        /**
          * return the number of outlet temperatures
          */
        int get_n_T_out()
        {
            return N_OUT;
        }

    private:

        /**
          * array of inlet temperature 
          */
        double * T_in; 

        /**
          * array of outlet temperature
          */
        double * T_out; 

        const int N_IN; 

        const int N_OUT;

        const BHE_NET_ELE _ele_type; 
        
    };





}

#endif
