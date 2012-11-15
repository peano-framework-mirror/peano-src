#include "peano/grid/AscendDescendLevelEnumerator.h"
#include "peano/grid/SingleLevelEnumerator.h"
#include "tarch/compiler/CompilerSpecificSettings.h"
#include "peano/utils/Loop.h"
#include "tarch/Assertions.h"


peano::grid::AscendDescendLevelEnumerator::AscendDescendLevelEnumerator(const SingleLevelEnumerator&  cloneFineGridEnumerator):
  _fineGridCellSize(cloneFineGridEnumerator._fineGridCellSize),
  _domainOffset(cloneFineGridEnumerator._domainOffset),
  _level(cloneFineGridEnumerator._level),
  _adjacentCellsHeight(cloneFineGridEnumerator._adjacentCellsHeight) {
}


peano::grid::AscendDescendLevelEnumerator::~AscendDescendLevelEnumerator() {
}


int peano::grid::AscendDescendLevelEnumerator::operator() (int localVertexNumber) const {
  assertion1( localVertexNumber>=0, localVertexNumber );
  assertion1( localVertexNumber<FOUR_POWER_D, localVertexNumber );
  return localVertexNumber;
}


int peano::grid::AscendDescendLevelEnumerator::operator() (const LocalVertexIntegerIndex&  localVertexNumber ) const {
  return SingleLevelEnumerator::lineariseVertexIndex(localVertexNumber);
}


int peano::grid::AscendDescendLevelEnumerator::operator() (const LocalVertexBitsetIndex&   localVertexNumber ) const {
  return (*this)( static_cast<int>( localVertexNumber.to_ulong()) );
}


peano::grid::AscendDescendLevelEnumerator::Vector peano::grid::AscendDescendLevelEnumerator::getVertexPosition(int localVertexNumber) const {
  return getVertexPosition(peano::utils::dDelinearised(localVertexNumber, 2));
}


peano::grid::AscendDescendLevelEnumerator::Vector peano::grid::AscendDescendLevelEnumerator::getVertexPosition(const LocalVertexIntegerIndex& localVertexNumber ) const {
  peano::grid::AscendDescendLevelEnumerator::Vector result( _domainOffset );
  #ifdef CompilerICC
  #pragma unroll (DIMENSIONS)
  #endif
  for(int d=0; d<DIMENSIONS;d++) {
	double delta = localVertexNumber(d);
	result(d) += delta * _fineGridCellSize(d);
  }
  return result;
}


peano::grid::AscendDescendLevelEnumerator::Vector peano::grid::AscendDescendLevelEnumerator::getVertexPosition(const LocalVertexBitsetIndex& localVertexNumber ) const {
  return getVertexPosition(static_cast<int>( localVertexNumber.to_ulong()) );
}


peano::grid::AscendDescendLevelEnumerator::Vector peano::grid::AscendDescendLevelEnumerator::getVertexPosition() const {
  return getVertexPosition(LocalVertexIntegerIndex(0));
}


peano::grid::AscendDescendLevelEnumerator::Vector peano::grid::AscendDescendLevelEnumerator::getCellCenter() const {
  return getVertexPosition() + _fineGridCellSize/2.0;
}


peano::grid::AscendDescendLevelEnumerator::Vector peano::grid::AscendDescendLevelEnumerator::getCellSize() const {
  return _fineGridCellSize;
}


std::string peano::grid::AscendDescendLevelEnumerator::toString() const {
  std::ostringstream out;
  out << "(domain-offset:" << _domainOffset
	  << ",cell-size:" << _fineGridCellSize
	  << ",level:" << _level
	  << ",adj-flags:" << _adjacentCellsHeight
	  << ")";
  return out.str();
}


int peano::grid::AscendDescendLevelEnumerator::getLevel() const {
  return _level;
}


peano::grid::CellFlags peano::grid::AscendDescendLevelEnumerator::getCellFlags() const {
  return _adjacentCellsHeight;
}


int peano::grid::AscendDescendLevelEnumerator::cell(const LocalVertexIntegerIndex& localVertexNumber) const {
  int result = 0;
  int base   = 1;
  #ifdef CompilerICC
  #pragma unroll (DIMENSIONS)
  #endif
  for(int d=0; d<DIMENSIONS;d++) {
    result += localVertexNumber(d) * base;
    base   *= 3;
  }
  return result;
}

