#include "peano/MappingSpecification.h"
#include "tarch/Assertions.h"


peano::MappingSpecification::MappingSpecification(peano::MappingSpecification::Manipulates manipulates_, bool multithreading_, bool restartable_):
  manipulates(manipulates_),
  multithreading(multithreading_),
  restartable(restartable_) {
  assertion(!restartable || multithreading);
}


peano::MappingSpecification operator&(const peano::MappingSpecification& lhs, const peano::MappingSpecification& rhs) {

  peano::MappingSpecification::Manipulates manipulates =
    (rhs.manipulates==peano::MappingSpecification::WHOLE_TREE  || lhs.manipulates==peano::MappingSpecification::WHOLE_TREE)  ? peano::MappingSpecification::WHOLE_TREE  :
    (rhs.manipulates==peano::MappingSpecification::ONLY_LEAVES || lhs.manipulates==peano::MappingSpecification::ONLY_LEAVES) ? peano::MappingSpecification::ONLY_LEAVES :
    peano::MappingSpecification::NOP;

  bool multithreading = lhs.multithreading && rhs.multithreading;
  bool restartable    = lhs.restartable    && rhs.restartable;

  return peano::MappingSpecification(manipulates,multithreading,restartable);
}


peano::MappingSpecification peano::MappingSpecification::getMostGeneralSpecification() {
  return MappingSpecification(NOP,true,true);
}
