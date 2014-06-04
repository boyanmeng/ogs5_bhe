/**
* \file BHE_2U.h
* 2014/06/04 HS inital implementation
* class of borehole heat exchanger with 2 U-tube
*
*/

#ifndef BHE_2U_H
#define BHE_2U_H

#include "BHEAbstract.h"

namespace BHE  // namespace of borehole heat exchanger
{

	class BHE_2U : public BHEAbstract
	{
	public:
		/**
		  * constructor
		  */
		BHE_2U() : BHEAbstract( BHE::BHE_TYPE_2U )
		{}; 

		/**
		  * return the number of unknowns needed for 2U BHE
		  */
		std::size_t get_n_unknowns() { return 8; }
		
	private:
		
	};


}  // end of namespace

#endif
