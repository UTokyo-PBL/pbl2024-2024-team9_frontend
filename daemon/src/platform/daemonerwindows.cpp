#ifdef WINDOWS_BUILD

#include <iostream>
#include <windows.h>
#include "daemon/platform/daemoner.h"

void daemonize() {
  std::cout << "windows daemon" << std::endl;
}

#endif
