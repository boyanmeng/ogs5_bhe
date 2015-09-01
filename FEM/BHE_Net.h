/**
* \file BHE_Net.h
* 2015/08/28 HS inital implementation
* borehole heat exchanger network class
*
*
*/

#ifndef BHE_NET_H
#define BHE_NET_H

#include <map>
#include "BHE_Net_ELE_Abstract.h"
#include "BHE_Net_ELE_Pipe.h"

namespace BHE  // namespace of borehole heat exchanger
{
    class BHE_Net {
    public:
        /**
          * constructor
          */
        BHE_Net(); 

        void add_bhe_net_elem(BHE_Net_ELE_Abstract* element);

        void add_bhe_net_pipe(BHE_Net_ELE_Pipe* pipe, 
                              std::string & from,
                              std::string & to);

        /**
          * get the number of unknowns
          */
        int get_n_unknowns(); 

        /**
          * get the number of elements in the network
          */
        int get_n_elems(); 

        /**
          * set the global indices for all elements in the network
          */
        void set_network_elem_global_idx(long n_nodes, long n_dofs_BHE);

    private:

        void count_n_unknowns();

        typedef std::map<std::string, BHE_Net_ELE_Abstract*> bhe_map;

        /**
          * a map including all bhes, distributors, and pipelines
          */
        bhe_map _bhe_net;

        /**
          * number of unknown temperatures in the network
          */
        int n_unknowns; 

    };
}

#endif