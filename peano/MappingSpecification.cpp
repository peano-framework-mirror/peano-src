#include "peano/MappingSpecification.h"


peano::MappingSpecification::MappingSpecification(peano::MappingSpecification::Manipulates manipulates_, bool multithreading_, bool restartable_):
  manipulates(manipulates_),
  multithreading(multithreading_),
  restartable(restartable_) {
}
