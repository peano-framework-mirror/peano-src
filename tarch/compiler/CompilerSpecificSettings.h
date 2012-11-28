// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_COMPILER_COMPILER_SPECIFIC_SETTINGS
#define _TARCH_COMPILER_COMPILER_SPECIFIC_SETTINGS


  #if defined(__INTEL_COMPILER) && defined(__WIN32__)
    #include "tarch/compiler/WindowsIntel.h"
  #elif defined(__GNUC__) && defined(__WIN32__)
    #include "tarch/compiler/WindowsGCC.h"
  #elif defined(__INTEL_COMPILER) && defined(__LINUX__)
    #include "tarch/compiler/LinuxIntel.h"
  #elif defined(__GNUC__) && defined(__LINUX__)
    #include "tarch/compiler/LinuxGCC.h"
  #elif defined(__APPLE__)
    #error CompilerSpecificSettings.h for Apple missing
  #else
    #error Unknown compiler target. Please adopt CompilerSpecificSettings.h
  #endif


#endif
