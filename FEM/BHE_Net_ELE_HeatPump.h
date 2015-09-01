/**
* \file BHE_Net_ELE_HeatPump.h
* 2015/08/28 HS inital implementation
* borehole heat exchanger network element heat pump class
*
*
*/

#ifndef BHE_NET_ELE_HEATPUMP_H
#define BHE_NET_ELE_HEATPUMP_H

#include "BHE_Net_ELE_Abstract.h"

namespace BHE  // namespace of borehole heat exchanger
{
	enum HEAT_PUMP_BOUNDARY_TYPE {
		HEAT_PUMP_BOUND_POWER_FIXED_FLOWRATE,
		HEAT_PUMP_BOUND_POWER_FIXED_DT,
	};

    class BHE_Net_ELE_HeatPump : public BHE_Net_ELE_Abstract {

        public: 
            BHE_Net_ELE_HeatPump(std::string & name);
                    

    };
}

#endif