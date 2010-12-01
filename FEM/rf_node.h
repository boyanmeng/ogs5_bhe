/**************************************************************************
FEMLib - Object: Node
Task: class implementation
Programing:
04/2004 OK Implementation
last modified
02/2005 MB node parameter....
**************************************************************************/
#ifndef rf_node_INC
#define rf_node_INC

// C++ STL
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// FEM
#include "FEMEnums.h"

class CNodeValue
{
  public:
	CNodeValue(void);
    ~CNodeValue(void);

    void setProcessDistributionType (DistributionType distype) { _node_distype = distype; }
    DistributionType getProcessDistributionType () const { return _node_distype; }
 //
    long geo_node_number;
    long msh_node_number;
    double node_value;
    double node_area;

//    int node_distype;
    double node_parameterA;
    double node_parameterB;
    double node_parameterC;
    double node_parameterD;
    double node_parameterE;
    int CurveIndex;
    int conditional;
    //std::vector<double>history_value;
    long msh_node_number_conditional;
	std::vector<long> msh_node_numbers_averaging;  // JOD   st-coupling 4.7.10
	std::vector<double> msh_node_weights_averaging; // JOD
	std::string tim_type_name;
    void Write(std::ostream& os=std::cout) const; //WW
    void Read(std::istream& is=std::cin); //WW
    bool check_me; //OK

  private:
	  DistributionType _node_distype;
};

#endif
