#include "peano/MappingSpecification.h"
#include "tarch/Assertions.h"


peano::MappingSpecification::MappingSpecification(peano::MappingSpecification::Manipulates manipulates_, bool multithreading_, bool restartable_):
  manipulates(manipulates_),
  multithreading(multithreading_),
  restartable(restartable_) {
  assertion(!restartable || multithreading);
}


peano::MappingSpecification operator+(const peano::MappingSpecification& lhs, const peano::MappingSpecification& rhs) {

  peano::MappingSpecification::Manipulates manipulates = peano::MappingSpecification::NOP;
  bool multithreading = lhs.multithreading && rhs.multithreading;
  bool restartable    = lhs.restartable    && rhs.restartable;

  return peano::MappingSpecification(manipulates,multithreading,restartable);
}
