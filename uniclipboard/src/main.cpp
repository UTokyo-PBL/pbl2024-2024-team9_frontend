#include <string>
#include "install.h"
#include "util.h"
#include "user.h"
#include "copy.h"
#include "paste.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    return 0;
  }

  std::ofstream outFile("log.txt");
  if (!outFile) {
    return 1;
  }
  // save the original buffer
  std::streambuf* orgbuf = std::cout.rdbuf();
  std::cout.rdbuf(outFile.rdbuf());

  auto args = ParseArguments(argc, argv);

  if (args.empty()) {
    return 0;
  }

  if (args.count("install")) {
    auto res = install(1, argv[0]);
    if (res != 0) {
      return res;
    }
  }

  if (args.count("uninstall")) {
    auto res = install(0, argv[0]);
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
    UniClipboard::CopyClipboardText();
  }

  if (args.count("file")) {
    auto path = args["file"];
    UniClipboard::CopyFromFile(path);
  }

  if (args.count("paste")) {
    auto path = args["paste"];
    UniClipboard::PasteToFile(path);
  }

  std::cout.rdbuf(orgbuf);
  outFile.close();

  return 0;
}
