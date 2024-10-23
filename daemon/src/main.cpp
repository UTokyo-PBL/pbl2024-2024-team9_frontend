#include <iostream>
#include "platform/daemoner.h"

#define WINDOWS

int main(int argc, char* argv[]) {
  std::cout << "initiating" << std::endl;
  daemonize();

  // TODO: functioning

  return 0;
}
