/**
* \file BHEAbstract.h
* 2014/06/04 HS inital implementation
* borehole heat exchanger abstract class
*
*/

#ifndef BHE_ABSTRACT_H
#define BHE_ABSTRACT_H

#include <iostream>

namespace BHE  // namespace of borehole heat exchanger
{

	class BHEAbstract
	{
	public:
		/**
		  * constructor
		  */
		BHEAbstract(BHE_TYPE my_type) 
			: type(my_type)
		{};

		/**
		  * destructor
		  */
		virtual ~BHEAbstract() {}; 

		/**
		  * return the number of unknowns needed for this BHE
		  * abstract function, need to be realized. 
		  */
		virtual std::size_t get_n_unknowns() = 0;

		/**
		  * return the type of the BHE
		  */
		BHE_TYPE get_type() { return type; };
	private:

		/**
		  * the type of the BHE
		  */
		const BHE_TYPE type; 
	};


	/**
	  * list the types of borehole heat exchanger
	  */
	enum BHE_TYPE {
		BHE_TYPE_2U,   // two u-tube borehole heat exchanger
		BHE_TYPE_1U,   // one u-tube borehole heat exchanger
		BHE_TYPE_CXC,  // coaxial pipe with annualar inlet
		BHE_TYPE_CXA	  // coaxial pipe with centreed inlet
	} ;

}  // end of namespace
#endif