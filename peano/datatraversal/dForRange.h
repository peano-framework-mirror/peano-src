// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_DFOR_RANGE_H_
#define _PEANO_DATA_TRAVERSAL_DFOR_RANGE_H_


#include "peano/utils/Globals.h"

#include "tarch/la/Vector.h"
#include "tarch/logging/Log.h"


#ifdef SharedTBB
#include <tbb/tbb_stddef.h>
#endif

namespace peano {
    namespace datatraversal {
      class dForRange;
    }
}



/**
 * Range implementation for the dFor.
 *
 * A range is a d-dimensional hypercube together with a translation. It
 * identifies an index set. Consequently, it is a generalisation of TBB's
 * blocked_range2d. Besides the data set to be traversed, each block also
 * has an attribute isSequentialBoundaryRange(). If this attribute holds,
 * the range does not describe a set that is to be traversed, but it
 * describes a cube whose boundary elements have to be traversed
 * deterministically, i.e. you are not allowed to split up this boundary
 * domain and process the two sets in parallel.
 *
 * The range basically implements an kd-tree-like approach: Whenever a split
 * is invoked (due to the constructor), it analyses along which coordinate
 * axis that cube is the longest. Then, it cuts the cube along this axis into
 * two parts of (besides rounding) the same size. A cube basically is
 * dividable as long as its volume is greater than zero, i.e. as long as it
 * contains more than one vertex along one coordinate axis.
 * However, the standard behaviour of the range also takes into account the
 * minimum grain size. Therefore a cube is not divided down to only one
 * vertex, but the division process may be stopped with a number of vertices
 * equal or larger than the given grain size. This is done due to
 * performance issues in multicore parallelization. An example of the
 * division process is illustrated below on the left-hand side.
 *
 * @image html dForRange.png
 *
 * If the original range is constructed with the handleBoundaryOfDomainSequential
 * flag, the behaviour is different. In such a case, the first split extracts
 * the boundary set. This set is not dividable anymore, and it is to be
 * traversed deterministally as isSequentialBoundaryRange() holds (red arrow in
 * the illustration). The remaining inner part of the domain is processed as
 * the domain described above, it can be split up further, and the individual
 * subdomains may by handled in parallel.
 *
 * The range hence not only is a generalisation of TBB's blocked_range2d. It
 * also adds this new semantics due to isSequentialBoundaryRange(). The latter
 * is an important attribute for example for hybrid parallelisation. Domain
 * boundaries here often are exchanged via message passing and the order on
 * the domain boundaries is fixed and has to be preserved. At the same time,
 * the vertices inside a domain can be processed non-deterministically in
 * parallel. Load balancing techniques for such an example are important for
 * the inner domain. On the boundary, we may not parallelise anythign and,
 * thus, load balancing (splitting) is not of relevance.
 *
 * You find more information on the original blocked_range2d at
 *
 * @see http://www.threadingbuildingblocks.org/uploads/81/91/Latest%20Open%20Source%20Documentation/Reference%20Manual%20(Open%20Source).pdf
 *
 * @author Wolfgang Eckhardt
 * @author Tobias Weinzierl
 */
class peano::datatraversal::dForRange {
  private:
    static tarch::logging::Log _log;

    tarch::la::Vector<DIMENSIONS,int>  _offset;
    tarch::la::Vector<DIMENSIONS,int>  _range;
    int                                _grainSize;
    bool                               _handleBoundaryOfDomainSequential;
    bool                               _isBoundaryRange;
  public:
    #ifdef SharedTBB
    typedef tbb::split Split;
    #else
    typedef std::string Split;
    #endif

    dForRange(const dForRange& range);

    /**
     * Construct a Complete Range
     */
    dForRange( const tarch::la::Vector<DIMENSIONS,int>&  range, int grainSize, bool handleBoundaryOfDomainSequential );

    /**
     * Split Constructor
     *
     * According to the TBB documentation, the split argument ain't used but is
     * only there do distinguish the operation from a copy constructor. The
     * passed argument is the valid state from which the new range is to be
     * constructed.
     *
     * If we have to treat the boundary separately and as a whole, the new
     * object becomes the boundary object. The argument range becomes a standard
     * range that we might be allowed to split further.
     *
     * @param range Original range
     */
    dForRange( dForRange& range, Split );

    /**
     * Is the range empty?
     *
     * A range is empty, if the corresponding volume of the hyperhexahedron
     * equals zero. If it is empty, it is also not divisible.
     */
    bool empty() const;

    /**
     * A range is divisible, if the volume of the corresponding range
     * hyperhexahedron is bigger than the grain size.
     */
    bool is_divisible() const;

    tarch::la::Vector<DIMENSIONS,int> getOffset() const;
    tarch::la::Vector<DIMENSIONS,int> getRange() const;

    std::string toString() const;

    bool isSequentialBoundaryRange() const;
};



#endif
