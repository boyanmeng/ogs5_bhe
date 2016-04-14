#include "BHE_Net_ELE_Pipe_Inner_1U.h"

using namespace BHE;

BHE_Net_ELE_Pipe_Inner_1U::BHE_Net_ELE_Pipe_Inner_1U(std::string & name, BHE::BHEAbstract * m_BHE)
    : BHE_Net_ELE_Abstract(name, BHE_NET_ELE::BHE_NET_PIPE_INNER_1U, 1, 1), 
    _penalty_factor(1.0e4), _m_BHE(m_BHE)
{

    // TODO configure the global indices

}

std::size_t BHE::BHE_Net_ELE_Pipe_Inner_1U::get_global_idx_in()
{
    return _global_idx_in;
}

std::size_t BHE::BHE_Net_ELE_Pipe_Inner_1U::get_global_idx_out()
{
    return _global_idx_out;
}

