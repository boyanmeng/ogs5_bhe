#include "BHE_Net_ELE_HeatPump.h"

using namespace BHE;

BHE_Net_ELE_HeatPump::BHE_Net_ELE_HeatPump(std::string & name)
    : BHE_Net_ELE_Abstract(name, BHE_NET_ELE::BHE_NET_HEATPUMP, 1, 1)
{
    _heat_pump_BC_type = HEAT_PUMP_BOUND_POWER_FIXED_DT; 
}

double BHE_Net_ELE_HeatPump::get_RHS_value()
{
    double rt_RHS_val = 0.0;

    // depending on the boundary condition, 
    // calculate the RHS value
    switch (_heat_pump_BC_type)
    {
    case HEAT_PUMP_BOUND_POWER_FIXED_DT:
        rt_RHS_val = _delta_T_val;
        break;
    case HEAT_PUMP_BOUND_POWER_FIXED_FLOWRATE:
        // TODO
        break;
    }
    return rt_RHS_val;
}