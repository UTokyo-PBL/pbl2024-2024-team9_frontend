#ifdef WINDOWS_BUILD

#include <iostream>
#include "daemoner.h"
#include <windows.h>

void daemonize() {
  std::cout << "windows daemon" << std::endl;
}

#endif
