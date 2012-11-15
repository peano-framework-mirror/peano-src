// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_LOOPS_SET_COUNTER__LOOP_BODY_H_
#define _PEANO_GRID_NODES_LOOPS_SET_COUNTER__LOOP_BODY_H_


#include "peano/utils/Globals.h"
#include "peano/geometry/Geometry.h"
#include "peano/grid/SingleLevelEnumerator.h"


namespace peano {
  namespace grid {
    namespace nodes {
      namespace loops {
        template <class Vertex>
        class SetCounterLoopBody;
      }
    }
    namespace tests {
      class SetCounterTest;
    }
  }
}


template <class Vertex>
class peano::grid::nodes::loops::SetCounterLoopBody {
  private:
    friend class peano::grid::tests::SetCounterTest;

    Vertex const * const                                        _coarseGridVertices;
    const peano::grid::SingleLevelEnumerator&  _coarseGridVerticesEnumerator;
    int * const                                                 _fineGridCounter;

    /**
     * Is vertex a hanging node or is it a new vertex?
     *
     * Determines whether a vertex is a hanging node or not. The operation is
     * passed the vertices of the father cell and a coordinate of the node to
     * check. The coordinate is specified using a discrete
     * @f$ \{ 0, \dots , 3 \}^d @f$ identifier.
     *
     * @return peano::grid::nodes::Constants
     */
    static int analyseFineGridVertex(
      Vertex const * const                                        coarseVertices,
      const peano::grid::SingleLevelEnumerator&  coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&                    coordinates
    );


    /**
     * Internal helper used by the other analyseFineGridVertex() operation.
     */
    static void analyseFineGridVertex(
      Vertex const * const                                        coarseVertices,
      const peano::grid::SingleLevelEnumerator&  coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&                    coordinates,
      tarch::la::Vector<DIMENSIONS,int>                           parentCoordinates,
      bool&                                                       oneFatherCarriesDeleteFlag,
      bool&                                                       oneFatherCarriesRefiningFlag,
      bool&                                                       oneFatherCarriesRefinedFlag,
      int                                                         dimension
    );

    static int analyseFineGridVertex(
      bool oneFatherCarriesDeleteFlag,
      bool oneFatherCarriesRefiningFlag,
      bool oneFatherCarriesRefinedFlag
    );
  public:
    SetCounterLoopBody(
      Vertex const * const                                        coarseGridVertices,
      const peano::grid::SingleLevelEnumerator&  coarseGridVerticesEnumerator,
      int * const                                                 fineGridCounter
    );

    /**
     * Set Counter at i
     */
    void operator() (const tarch::la::Vector<DIMENSIONS,int>& i) const;
};


#include "peano/grid/nodes/loops/SetCounterLoopBody.cpph"


#endif
