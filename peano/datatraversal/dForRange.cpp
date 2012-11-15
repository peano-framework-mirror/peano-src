#include "peano/datatraversal/dForRange.h"


tarch::logging::Log peano::datatraversal::dForRange::_log( "peano::datatraversal::dForRange" );


peano::datatraversal::dForRange::dForRange(const dForRange& range):
  _offset( range._offset),
  _range( range._range ),
  _grainSize( range._grainSize ),
  _handleBoundaryOfDomainSequential(range._handleBoundaryOfDomainSequential),
  _isBoundaryRange( range._isBoundaryRange ) {
  logTraceInWith1Argument( "dForRange(dForRange)", range.toString() );
  logTraceOut( "dForRange(dForRange)" );
}


peano::datatraversal::dForRange::dForRange( const tarch::la::Vector<DIMENSIONS,int>&  range, int grainSize, bool handleBoundaryOfDomainSequential ):
  _offset(0),
  _range(range),
  _grainSize(grainSize),
  _handleBoundaryOfDomainSequential(handleBoundaryOfDomainSequential),
  _isBoundaryRange(false) {
  logTraceInWith2Arguments( "dForRange(...)", range, grainSize );
  for (int d=0; d<DIMENSIONS; d++) {
    assertion( range(d)>=1 );
  }
  if (handleBoundaryOfDomainSequential) {
    for (int d=0; d<DIMENSIONS; d++) {
      assertion( range(d)>=1 );
      if (_range(d)<2) _isBoundaryRange = true;
    }
  }
  logTraceOut( "dForRange(...)" );
}


bool peano::datatraversal::dForRange::isSequentialBoundaryRange() const {
  return _handleBoundaryOfDomainSequential;
}


peano::datatraversal::dForRange::dForRange( dForRange& range, Split ):
  _offset(range._offset),
  _range(range._range),
  _grainSize( range._grainSize),
  _handleBoundaryOfDomainSequential( range._handleBoundaryOfDomainSequential ),
  _isBoundaryRange( range._isBoundaryRange ) {
  logTraceInWith1Argument( "dForRange(Range,Split)", range.toString() );

  assertion(!range._isBoundaryRange);
  assertion(range.is_divisible());

  if (_handleBoundaryOfDomainSequential) {
    _isBoundaryRange                        = true;
    range._isBoundaryRange                  = false;
    range._handleBoundaryOfDomainSequential = false;
    range._offset += 1;
    range._range  -= 2;
    assertion( tarch::la::volume(range._range)>0);
  }
  else {
    int splitAlongDimension = tarch::la::indexMax(_range);

    int oldRangeSizeAlongDimension = _range(splitAlongDimension);
    _range(splitAlongDimension) =  oldRangeSizeAlongDimension / 2;

    range._range(splitAlongDimension)  = oldRangeSizeAlongDimension - _range(splitAlongDimension);
    range._offset(splitAlongDimension) = range._offset(splitAlongDimension) + oldRangeSizeAlongDimension - range._range(splitAlongDimension);
  }

  logTraceOutWith2Arguments( "dForRange(Range,Split)", toString(), range.toString() );
}


bool peano::datatraversal::dForRange::empty() const {
  logTraceIn( "empty()" );
  bool result = tarch::la::volume(_range)==0;
  assertion( !is_divisible() || !result );
  logTraceOutWith1Argument( "empty()", result );
  return result;
}


bool peano::datatraversal::dForRange::is_divisible() const {
  logTraceInWith1Argument( "is_divisible()", toString() );
  bool result = tarch::la::volume(_range) > _grainSize && !_isBoundaryRange;
  logTraceOutWith1Argument( "is_divisible()", result );
  return result;
}


std::string peano::datatraversal::dForRange::toString() const {
  std::ostringstream msg;
  msg << "(range:" << _range << ",offset:" << _offset << ",grain-size:" << _grainSize << ")";
  return msg.str();
}


tarch::la::Vector<DIMENSIONS,int> peano::datatraversal::dForRange::getOffset() const {
  return _offset;
}


tarch::la::Vector<DIMENSIONS,int> peano::datatraversal::dForRange::getRange() const {
  return _range;
}
