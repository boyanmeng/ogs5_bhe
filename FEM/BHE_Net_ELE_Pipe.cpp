#include "BHE_Net_ELE_Pipe.h"

using namespace BHE;

BHE_Net_ELE_Pipe::BHE_Net_ELE_Pipe(std::string & name)
    : BHE_Net_ELE_Abstract(name, BHE_NET_ELE::BHE_NET_PIPE, 1, 1)
{

}

double BHE_Net_ELE_Pipe::get_RHS_value()
{
    return 0; 
}