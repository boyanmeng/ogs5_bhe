/*
 * Surface.h
 *
 *  Created on: Jan 22, 2010
 *      Author: TF
 */

#ifndef SURFACE_H_
#define SURFACE_H_

#include <vector>
#include "Point.h"
#include "Triangle.h"
#include "AxisAlignedBoundingBox.h"

namespace GEOLIB {

/** \brief A Surface is represented by Triangles. It consists of a reference
 * to a vector of (pointers to) points (m_sfc_pnts) and a vector that stores
 * the Triangles consisting of points from m_sfc_pnts.
 * */
class Surface
{
public:
	Surface	(const std::vector<Point*> &pnt_vec);
	virtual ~Surface ();

	/**
	 * adds three indices describing a triangle and updates the bounding box
	 * */
	void addTriangle (size_t pnt_a, size_t pnt_b, size_t pnt_c);

	/**
	 * returns the number of triangles describing the Surface
	 * */
	size_t getNTriangles () const;

	/** \brief const access operator for the access to the i-th Triangle of the surface.
	*/
	const Triangle* operator[] (size_t i) const;

	/**
	 * is the given point in the bounding volume of the surface
	 */
	bool isPntInBV (const double *pnt) const;

	/**
	 * is the given point located in the surface
	 */
	bool isPntInSfc (const double *pnt) const;

	const std::vector<Point*> *getPointVec() const { return &m_sfc_pnts; };


protected:
	/** a vector of pointers to Points */
	const std::vector<Point*> &m_sfc_pnts;
	/** position of pointers to the geometric points */
	std::vector<Triangle*> m_sfc_triangles;
	/** bounding volume is an axis aligned bounding box */
	AABB *bv;
};

}

#endif /* SURFACE_H_ */
