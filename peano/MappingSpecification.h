// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_MAPPING_SPECIFICATION_H_
#define _PEANO_MAPPING_SPECIFICATION_H_


namespace peano {
  struct MappingSpecification;
}


struct peano::MappingSpecification {
  enum Manipulates  {
    NOP, ONLY_LEAVES, WHOLE_TREE
  };

  Manipulates manipulates;
  bool        multithreading;
  bool        restartable;

  MappingSpecification(Manipulates manipulates_, bool _multithreading_, bool _restartable_);
};

#endif
