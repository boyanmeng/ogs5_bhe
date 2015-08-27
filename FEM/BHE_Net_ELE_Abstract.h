/**
* \file BHE_Net_ELE_Abstract.h
* 2015/08/27 HS inital implementation
* borehole heat exchanger network element abstract class
*
*
*/

#ifndef BHE_NET_ELE_ABSTRACT_H
#define BHE_NET_ELE_ABSTRACT_H

namespace BHE  // namespace of borehole heat exchanger
{

    class BHE_Net_ELE_Abstract {
    public:

        /**
          * constructor
          */
        BHE_Net_ELE_Abstract(int n_inlet = 1, int n_outlet = 1){

            int i; 
            // initialize T_in
            T_in = new double[n_inlet];
            for (i = 0; i < n_inlet; i++)
            {
                T_in[i] = 0.0;
            }

            // initialize T_out
            T_out = new double[n_outlet];
            for (i = 0; i < n_outlet; i++)
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

    private:

        /**
          * array of inlet temperature 
          */
        double * T_in; 

        /**
          * array of outlet temperature
          */
        double * T_out; 

        
    };





}

#endif
