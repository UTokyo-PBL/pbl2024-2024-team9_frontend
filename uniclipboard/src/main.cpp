#include <string>
#include "install.h"
#include "util.h"
#include "user.h"
#include "copy.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    return 0;
  }

  auto args = ParseArguments(argc, argv);

  if (args.empty()) {
    return 1;
  }

  if (args.count("install")) {
    auto programPath = args["install"];
    auto res = install(1, programPath);
    if (res != 0) {
      return res;
    }
  }

  if (args.count("uninstall")) {
    auto programPath = args["uninstall"];
    auto res = install(0, programPath);
    if (res != 0) {
      return res;
    }
  }

  if (args.count("login")) {
    auto uname = std::string("testuser");
    auto pwd = std::string("test123");
    UserLogin(uname, pwd);
  }

  if (args.count("copy")) {
    auto text = std::string("some random text");
    CopyText(text);
  }

  return 0;
}
