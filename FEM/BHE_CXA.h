/**
* \file BHE_CXA.h
* 2014/06/04 HS inital implementation
* class of borehole heat exchanger with annular coaxial pipe
*
*/

#ifndef BHE_CXA_H
#define BHE_CXA_H

#include "BHEAbstract.h"

namespace BHE  // namespace of borehole heat exchanger
{

	class BHE_CXA : public BHEAbstract
	{
	public:
		/**
		  * constructor
		  */
		BHE_CXA() : BHEAbstract(BHE::BHE_TYPE_CXA)
		{};



	private:

	};


}  // end of namespace

#endif