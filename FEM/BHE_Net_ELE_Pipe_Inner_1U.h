/**
* \file BHE_Net_ELE_Pipe_Inner_1U.h
* 2016/04/14 HS inital implementation
* borehole heat exchanger network element pipeline class
* particularly designed for 1U type of BHE
* used to connect the bottom of BHE, linking the inlet and 
* outlet pipelines
*/

#ifndef BHE_NET_ELE_PIPE_INNER_1U_H
#define BHE_NET_ELE_PIPE_INNER_1U_H

#include "BHE_Net_ELE_Abstract.h"

namespace BHE  // namespace of borehole heat exchanger
{
    class BHE_Net_ELE_Pipe_Inner_1U : public BHE_Net_ELE_Abstract {

    public:
        /**
        * constructor
        */
        BHE_Net_ELE_Pipe_Inner_1U(std::string & name);



    };

}

#endif