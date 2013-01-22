// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _GEOMETRY_GEOMETRY_H_
#define _GEOMETRY_GEOMETRY_H_
#include "tarch/la/Vector.h"
#include "peano/utils/Globals.h"


namespace peano {
  namespace geometry {
    class Geometry;

    /** currently each geometry object is identical with the overall geometry interface.
     *  However, a single geometry object might obtain more properties in future implementations,
     *  still keeping all characteristics of Geometry (GeometryObject inherits from Geometry).
     */
    typedef Geometry GeometryObject;
  }
}


/**
 * Interface for builtin or other geometry descriptions
 *
 * @author Philipp Neumann, Tobias Weinzierl, Tobias Neckel
 */
class peano::geometry::Geometry {
  public:
    virtual ~Geometry(){}

    /**
     * Routine for decision if the point and the surrounding box lies outside
     * the geometry.
     *
     * With value zero for the parameter resolution, this gives the exact
     * geometry information of a single point position x (outside yes or no).
     *
     * Please note that the computational domain splits up the whole
     * space @f$ \mathbf{R}^d @f$ into outside, inside and a set of points
     * (submanifold) that are neither inside nor outside. Thus, a pointwise
     * query (h equals the zero vector) returns one of three values mapped onto
     * the two functions isCompletelyInside() and isCompletelyOutside(). The
     * spatial query, where the query point x is added a surrounding box with
     * size 2*resolution where x is in the center is a little bit more complicated: The surrounding box
     * is interpreted to be an open domain and isCompletelyInside() /
     * isCompletelyOutside() returns true iff the intersection of the inner /
     * outer domain with the open surrounding box is the box itself.
     *
     * @image html ../kernel/gridinterface/geometry-vertex-inside-outside.png
     * @image html ../kernel/gridinterface/geometry-cell-inside-outside.png
     *
     * The query is not const anymore, as PeGSI might want to change the
     * underlying spacetree structure.
     *
     */
     virtual bool isCompletelyOutside( const tarch::la::Vector<DIMENSIONS,double>& x, const tarch::la::Vector<DIMENSIONS,double> &resolution ) = 0;

    /**
     * Routine for decision if the point and the surrounding box lies inside
     * the geometry.
     *
     * With value zero for the parameter resolution, this gives the exact
     * geometry information of a single point position x (outside yes or no).
     *
     * Please note that the computational domain splits up the whole
     * space @f$ \mathbf{R}^d @f$ into outside, inside and a set of points
     * (submanifold) that are neither inside nor outside. Thus, a pointwise
     * query (h equals the zero vector) returns one of three values mapped onto
     * the two functions isCompletelyInside() and isCompletelyOutside(). The
     * spatial query, where the query point x is added a surrounding box with
     * size 2*resolution where x is in the center is a little bit more complicated: The surrounding box
     * is interpreted to be an open domain and isCompletelyInside() /
     * isCompletelyOutside() returns true iff the intersection of the inner /
     * outer domain with the open surrounding box is the box itself.
     *
     * @image html ../kernel/gridinterface/geometry-vertex-inside-outside.png
     * @image html ../kernel/gridinterface/geometry-cell-inside-outside.png
     *
     * The query is not const anymore, as PeGSI might want to change the
     * underlying spacetree structure.
     *
     */
    virtual bool isCompletelyInside( const tarch::la::Vector<DIMENSIONS,double>& x, const tarch::la::Vector<DIMENSIONS,double> &resolution ) = 0;

    /**
     * @todo Well could someone write a description here?
     */
    virtual bool isOutsideClosedDomain( const tarch::la::Vector<DIMENSIONS,double>& x ) = 0;

    /**
     * Ask geometry whether subdomain has changed.
     *
     * @param x          Center of subdomain
     * @param resolution Resolution of subdomain, i.e. hexahedron surrounding x
     */
    virtual bool domainHasChanged( const tarch::la::Vector<DIMENSIONS,double>& x, const tarch::la::Vector<DIMENSIONS,double> &resolution ) = 0;

    /**
     * Refine grid intersecting cells at the cell boundary
     *
     * Peano identifies whenever a cell is intersected by the computational
     * domain but all its vertices are outside. In this case, the vertices
     * probably never would refine. But Peano identifies this situation and
     * asks the geometry due to this operation whether it shall refine here.
     * In this routine, at least the minimum mesh size should be checked.
     *
     * @image html refineOuterCellWithExclusivelyOuterVerticesAsItIntersectsDomain.png
     *
     * @param x          Center of subdomain
     * @param resolution Resolution of subdomain, i.e. hexahedron surrounding x
     *
     * !!! Remark
     *
     * This operation should care about something like a characteristic length
     * of a setup. If the h is smaller than this length, it probably should
     * return false, but if this does not hold, it should return true to force
     * the implementation to refine and to resolve all characteristic scales.
     */
    virtual bool refineOuterCellWithExclusivelyOuterVerticesAsItIntersectsDomain( const tarch::la::Vector<DIMENSIONS,double>& x, const tarch::la::Vector<DIMENSIONS,double> &resolution ) const = 0;
};

#endif
