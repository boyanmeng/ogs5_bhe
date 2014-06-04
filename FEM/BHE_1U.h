/**
* \file BHE_1U.h
* 2014/06/04 HS inital implementation
* class of borehole heat exchanger with 1 U-tube
*
*/

#ifndef BHE_1U_H
#define BHE_1U_H

#include "BHEAbstract.h"

namespace BHE  // namespace of borehole heat exchanger
{

	class BHE_1U : public BHEAbstract
	{
	public:
		/**
		  * constructor
		  */
		BHE_1U() : BHEAbstract(BHE::BHE_TYPE_1U)
		{};

		/**
		  * return the number of unknowns needed for 1U BHE
		  */
		std::size_t get_n_unknowns() { return 4; }

	private:

	};


}  // end of namespace

#endif