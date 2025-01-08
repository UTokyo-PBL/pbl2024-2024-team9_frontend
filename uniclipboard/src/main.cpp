#include "util.h"

int main(int argc, char* argv[]) {
  ShowWindow(GetConsoleWindow(), SW_HIDE);

  std::ofstream outFile("log.txt");
  if (!outFile) {
    return 1;
  }
  // save the original buffer
  std::streambuf* orgbuf = std::cout.rdbuf();
  std::cout.rdbuf(outFile.rdbuf());

  auto result = UniClipboard::run(argc, argv);

  std::cout.rdbuf(orgbuf);
  outFile.close();

  return result;
}
