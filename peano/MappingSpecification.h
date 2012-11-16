// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_MAPPING_SPECIFICATION_H_
#define _PEANO_MAPPING_SPECIFICATION_H_


namespace peano {
  struct MappingSpecification;
}


/**
 * Specification for a mapping operation
 *
 * Gives Peano hints on the behaviour and, hence, supports a more aggressive
 * on-the-fly optimisation.
 *
 *
|| manipulates || NOP | Operation is empty, and the algorithm's semantics is preserved, if Peano kicks out the whole function call.
||             || ONLY_LEAVES | Algorithm's semantics is preserved, if Peano kicks out the whole function call for refined vertices of cells that have solely refined vertices around them.
||             || WHOLE_TREE  | Default. No events are eliminated.
|| multithreading || false | Default. Do not run these operation in parallel on a shared memory machine. Application hence doesn't need semaphores.
||                || true  | Peano can try to speed up the application due to multithreading.
|| restartable    || false | Default.
||                || true  | If the operation is deployed to a thread of its own and this thread creashes, Peano may reexecute this operation. Consequently, some operations might be called multiple times due to a hardware failure, but the code is resiliency safe, i.e. does not crash due to simple hardware errors.
 *
 *
 *
 * @author Tobias Weinzierl
 */
struct peano::MappingSpecification {
  enum Manipulates  {
    NOP, ONLY_LEAVES, WHOLE_TREE
  };

  Manipulates manipulates;
  bool        multithreading;
  bool        restartable;

  MappingSpecification(Manipulates manipulates_, bool _multithreading_, bool _restartable_);
};

peano::MappingSpecification operator+(const peano::MappingSpecification& lhs, const peano::MappingSpecification& rhs);


#endif
