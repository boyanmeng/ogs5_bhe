#include "BHE_Net_ELE_HeatPump.h"

using namespace BHE;

BHE_Net_ELE_HeatPump::BHE_Net_ELE_HeatPump(std::string & name)
    : BHE_Net_ELE_Abstract(name, BHE_NET_ELE::BHE_NET_HEATPUMP, 1, 1)
{

}

double BHE_Net_ELE_HeatPump::get_RHS_value()
{
    double rt_RHS_val = 0.0;

    // TODO depending on the boundary condition, 
    // calculate the RHS value

    return rt_RHS_val;
}