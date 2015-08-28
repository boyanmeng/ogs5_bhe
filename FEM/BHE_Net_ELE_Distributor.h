/**
* \file BHE_Net_ELE_Distributor.h
* 2015/08/28 HS inital implementation
* borehole heat exchanger network element distributor class
*
*
*/

#ifndef BHE_NET_ELE_DISTRIBUTOR_H
#define BHE_NET_ELE_DISTRIBUTOR_H

#include "BHE_Net_ELE_Abstract.h"

namespace BHE  // namespace of borehole heat exchanger
{
    class BHE_Net_ELE_Distributor : public BHE_Net_ELE_Abstract 
    {
        public:
            BHE_Net_ELE_Distributor(Eigen::VectorXd & vec_Inlet_Ratio, Eigen::VectorXd & vec_Outlet_Ratio);

        private:
            /**
              * how the inlet flow rate is determined. 
              */
            Eigen::VectorXd _vec_inlet_ratio; 

            /**
              * how the outlet flow rate is determined. 
              */
            Eigen::VectorXd _vec_outlet_ratio;

    }; 
}

#endif