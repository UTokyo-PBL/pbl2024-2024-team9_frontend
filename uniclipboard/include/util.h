#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include "install.h"
#include "copy.h"
#include "paste.h"
#include "gui.h"

namespace UniClipboard {

using strmap = std::unordered_map<std::string, std::string>;

inline strmap ParseArguments(int argc, char* argv[]) {
  strmap arguments;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg.size() > 2 && arg.substr(0, 2) == "--") {
      std::string key = arg.substr(2);
      if (i + 1 < argc) {  // check for a value
        std::string nextArg = argv[i + 1];

        if (nextArg.size() > 2 && nextArg.substr(0, 2) == "--") {
          arguments[key] = "true";
          continue;
        }
        arguments[key] = nextArg;
        ++i;  // skip the value
      } else {
        arguments[key] = "true";
      }
    } else {
      std::cerr << "Warning: Ignoring invalid argument format: " << arg << std::endl;
    }
  }

  return arguments;
}

inline int run(int argc, char* argv[]) {
  if (argc < 2) {
    return 0;
  }

  auto args = ParseArguments(argc, argv);

  if (args.empty()) {
    return 0;
  }

  if (args.count("install")) {
    auto res = UniClipboard::install(1, argv[0]);
    if (res != 0) {
      return res;
    }
  }

  if (args.count("uninstall")) {
    auto res = UniClipboard::install(0, argv[0]);
    if (res != 0) {
      return res;
    }
  }

  if (args.count("login")) {
    UniClipboard::GUILogin();
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

  return 0;
}
}  // namespace UniClipboard
