#include "peano/MappingSpecification.h"
#include "tarch/Assertions.h"
#include "tarch/logging/Log.h"


#include <sstream>


peano::MappingSpecification::MappingSpecification(peano::MappingSpecification::Manipulates manipulates_, bool multithreading_, bool restartable_):
  manipulates(manipulates_),
  multithreading(multithreading_),
  restartable(restartable_) {
  assertion(!restartable || multithreading);
}


std::string peano::MappingSpecification::toString() const {
  std::ostringstream msg;

  msg << "(manipulates=";
  switch (manipulates) {
    case NOP:
      msg << "nop";
      break;
    case ONLY_LEAVES:
      msg << "only leaves";
      break;
    case WHOLE_TREE:
      msg << "whole tree";
      break;
  }
  msg << ",multithreading=" << multithreading
      << ",restartable=" << restartable
      << ")";
  return msg.str();
}


peano::MappingSpecification operator&(const peano::MappingSpecification& lhs, const peano::MappingSpecification& rhs) {
  static tarch::logging::Log _log( "peano::MappingSpecification" );
  logTraceInWith2Arguments("operator&()", lhs.toString(), rhs.toString() );

  peano::MappingSpecification::Manipulates manipulates =
    (rhs.manipulates==peano::MappingSpecification::WHOLE_TREE  || lhs.manipulates==peano::MappingSpecification::WHOLE_TREE)  ? peano::MappingSpecification::WHOLE_TREE  :
    (rhs.manipulates==peano::MappingSpecification::ONLY_LEAVES || lhs.manipulates==peano::MappingSpecification::ONLY_LEAVES) ? peano::MappingSpecification::ONLY_LEAVES :
    peano::MappingSpecification::NOP;

  bool multithreading = lhs.multithreading && rhs.multithreading;
  bool restartable    = lhs.restartable    && rhs.restartable;

  peano::MappingSpecification result(manipulates,multithreading,restartable);
  logTraceOutWith1Argument("operator&()",result.toString());
  return result;
}


peano::MappingSpecification peano::MappingSpecification::getMostGeneralSpecification() {
  return MappingSpecification(NOP,true,true);
}
