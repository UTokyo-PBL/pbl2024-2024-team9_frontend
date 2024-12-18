#include <iostream>
#include <string>
#include <unordered_map>

using strmap = std::unordered_map<std::string, std::string>;

inline strmap ParseArguments(int argc, char* argv[]) {
  strmap arguments;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg.size() > 2 && arg.substr(0, 2) == "--") {
      std::string key = arg.substr(2);
      if (i + 1 < argc) {  // Check for a value
        std::string nextArg = argv[i + 1];

        if (nextArg.size() > 2 && nextArg.substr(0, 2) == "--") {
          arguments[key] = "true";
          continue;
        }
        arguments[key] = nextArg;
        ++i;  // Skip the value
      } else {
        // Boolean flag
        arguments[key] = "true";
      }
    } else {
      std::cerr << "Warning: Ignoring invalid argument format: " << arg << std::endl;
    }
  }

  return arguments;
}
