#include <iostream>
#include "BHE_Net.h"

using namespace BHE; 

BHE_Net::BHE_Net()
{

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