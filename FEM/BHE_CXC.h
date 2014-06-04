/**
* \file BHE_CXC.h
* 2014/06/04 HS inital implementation
* class of borehole heat exchanger with centred coaxial pipe
*
*/

#ifndef BHE_CXC_H
#define BHE_CXC_H

#include "BHEAbstract.h"

namespace BHE  // namespace of borehole heat exchanger
{

	class BHE_CXC : public BHEAbstract
	{
	public:
		/**
		  * constructor
		  */
		BHE_CXC() : BHEAbstract(BHE::BHE_TYPE_CXC)
		{};

		/**
		  * return the number of unknowns needed for CXC BHE
		  */
		std::size_t get_n_unknowns() { return 3; }

	private:

	};


}  // end of namespace

#endif