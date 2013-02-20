// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_LA_MATRIXOPERATIONS_H_
#define _TARCH_LA_MATRIXOPERATIONS_H_


#include "tarch/la/Matrix.h"


#include <sstream>
#include <cmath>

namespace tarch {
  namespace la {
    /**
     * Computes the sum of all entries of the matrix.
     */
    template<int Rows, int Cols, typename Scalar>
    Scalar sum (const Matrix<Rows,Cols,Scalar>& matrix);


    template<int Rows, int Cols, typename Scalar>
    Matrix<Cols,Rows,Scalar> transpose(const Matrix<Rows,Cols,Scalar>& matrix);
  }
}



template<int Rows, int Cols, typename Scalar>
std::ostream&  operator<<(std::ostream& os, const tarch::la::Matrix<Rows,Cols,Scalar>&  matrix);


#include "tarch/la/MatrixOperations.cpph"

#endif /* _TARCH_LA_MATRIXOPERATIONS_H_ */
