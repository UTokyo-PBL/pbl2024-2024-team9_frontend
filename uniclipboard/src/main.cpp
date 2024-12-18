#include <ostream>
#include <string>
#include "install.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    return 0;
  }

  int regOption = std::stoi(argv[1]);
  std::string programPath = argv[2];

  if (regOption == 1) {
    if (!AddToContextMenu(programPath)) {
      return 1;
    }
  } else if (regOption == 0) {
    if (!RemoveFromContextMenu(programPath)) {
      return 1;
    }
  }

  return 0;
}
