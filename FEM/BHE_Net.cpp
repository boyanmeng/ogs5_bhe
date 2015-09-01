#include <iostream>
#include "BHE_Net.h"

using namespace BHE; 

BHE_Net::BHE_Net()
{
    n_unknowns = 0; 
}

void BHE_Net::add_bhe_net_elem(BHE_Net_ELE_Abstract* element)
{
    bhe_map::iterator itr = _bhe_net.begin();
    std::string name = element->get_ele_name();
    itr = _bhe_net.find(name);
    if (itr == _bhe_net.end())
    {
        _bhe_net[name] = element; 
        return;
    }
    std::cout << "BHE net element already exists!\n";
}

void BHE_Net::add_bhe_net_pipe(BHE_Net_ELE_Pipe* pipe,
                               std::string & from,
                               std::string & to)
{
    bhe_map::iterator itr      = _bhe_net.begin();
    bhe_map::iterator itr_from = _bhe_net.begin();
    bhe_map::iterator itr_to   = _bhe_net.begin();
    std::string name = pipe->get_ele_name();
    itr = _bhe_net.find(name);
    itr_from = _bhe_net.find(from); 
    itr_to   = _bhe_net.find(to);

    if (itr == _bhe_net.end())
    {
        // not having it in the map yet
        _bhe_net[name] = pipe;

        // check inlet link
        if (itr_from == _bhe_net.end())
        {
            // not existing
            std::cout << "BHE net pipeline inlet link does not exist!\n";
            exit(1);
        }
        else
        {
            pipe->add_inlet_connet(itr_from->second);
            itr_from->second->add_outlet_connet(pipe);
        }

        // check outlet link
        if (itr_to == _bhe_net.end())
        {
            // not existing
            std::cout << "BHE net pipeline outlet link does not exist!\n";
            exit(1);
        }
        else
        {
            pipe->add_outlet_connet(itr_to->second);
            itr_to->second->add_inlet_connet(pipe);
        }

        return;
    }
    else
    {
        std::cout << "BHE net pipeline already exists!\n";
        exit(1);
    }
}

void BHE_Net::count_n_unknowns()
{
    n_unknowns = 0;
    // loop over all elements in the map 
    typedef bhe_map::iterator it_type;
    for (it_type iterator = _bhe_net.begin(); iterator != _bhe_net.end(); iterator++) {
        // not counting the BHE, not counting the pipe
        if (iterator->second->get_net_ele_type() == BHE::BHE_NET_BOREHOLE || iterator->second->get_net_ele_type() == BHE::BHE_NET_PIPE)
            continue; 
        else
        {
            n_unknowns += iterator->second->get_n_T_in();
            n_unknowns += iterator->second->get_n_T_out();
        }
    }
}

int BHE_Net::get_n_unknowns()
{
    // first count
    count_n_unknowns();

    // then return
    return n_unknowns; 
}

int BHE_Net::get_n_elems()
{
    // return the number of elements in the network
    return _bhe_net.size(); 
}

void BHE_Net::set_network_elem_global_idx(long n_nodes, long n_dofs_BHE)
{
    int i; 
    long idx = n_nodes + n_dofs_BHE; 

    // loop over all elements in the map 
    typedef bhe_map::iterator it_type;
    for (it_type iterator = _bhe_net.begin(); iterator != _bhe_net.end(); iterator++) {
        // not counting the BHE, not counting the pipe
        if (iterator->second->get_net_ele_type() == BHE::BHE_NET_BOREHOLE || iterator->second->get_net_ele_type() == BHE::BHE_NET_PIPE)
            continue;
        else
        {
            // assgin index to T_in
            for (i = 0; i < iterator->second->get_n_T_in(); i++)
            {
                iterator->second->set_T_in_global_index(idx, i);
                idx++; 
            }

            // assgin index to T_out
            for (i = 0; i < iterator->second->get_n_T_out(); i++)
            {
                iterator->second->set_T_out_global_index(idx, i);
                idx++;
            }
        }
    }

    
}