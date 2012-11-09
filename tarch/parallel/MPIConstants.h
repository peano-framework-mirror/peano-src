// Copyright (C) 2009 Technische Universitaet Muenchen
// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www5.in.tum.de/peano
#ifndef _TARCH_PARALLEL_MPI_CONSTANTS_H_
#define _TARCH_PARALLEL_MPI_CONSTANTS_H_


#include "tarch/compiler/CompilerSpecificSettings.h"

#if !defined(CompilerDefinesMPIMaxNameString)
#define MPI_MAX_NAME_STRING            80
#endif


#define MPI_MAX_NAME_STRING_ADDED_ONE  (MPI_MAX_NAME_STRING+1)

#include "tarch/la/Vector.h"


namespace tarch {
  namespace parallel {
    class StringTools;
  }
}


/**
 * @todo Diese Klasse kommt raus, sobald der String im DaStGen funktioniert.
 */
class tarch::parallel::StringTools {
  private:
    StringTools() {}
    ~StringTools() {}
  public:
    static std::string convert(const tarch::la::Vector<MPI_MAX_NAME_STRING_ADDED_ONE,short int>& value );
    static tarch::la::Vector<MPI_MAX_NAME_STRING_ADDED_ONE,short int> convert( const std::string& value );
};

#endif
