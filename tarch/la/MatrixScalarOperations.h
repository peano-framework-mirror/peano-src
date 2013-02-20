// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_LA_MATRIXSCALAROPERATIONS_H_
#define _TARCH_LA_MATRIXSCALAROPERATIONS_H_


namespace tarch {
  namespace la {
    template<int Rows, int Cols, typename Scalar>
    Matrix<Rows,Cols,Scalar>& operator*= (
      Matrix<Rows,Cols,Scalar>&  matrix,
      const Scalar&              scalar
    );

    /**
      * Multiplies every component of the vector with the scalar and assigns the
      *  result to the vector.
      *
      * No temporary objects are created during the operation.
     */
    template<int Rows, int Cols, typename Scalar>
    Matrix<Rows,Cols,Scalar> operator*(
      const Matrix<Rows,Cols,Scalar>&  matrix,
      const Scalar&                    scalar
    );
  }
}

#include "tarch/la/MatrixScalarOperations.cpph"

#endif
