#pragma once
#include <string>
#include "netutil.h"
#include <windows.h>

namespace UniClipboard {

inline void PasteToFile(const std::string& path) {
  Json json = {};

  auto res = NetPull(json, "/list_all_items");
  if (!res) {
    return;
  }

  try {
    auto items = Json::parse(res->body);
    std::cout << res->body << std::endl;
    // todo: timestamp use unix format

    auto size = items.size();
    if (size < 1) {
      return;
    }

    std::string text = items[0]["content"];

    if (std::filesystem::path(path).extension() == ".txt") {
      std::ofstream outFile(path, std::ios::app);

      if (!outFile) {
        std::cerr << "Failed to paste: " << path << std::endl;
        return;
      }

      outFile << text;
      outFile.close();
      return;
    }

  } catch (const std::exception& e) {
    std::cerr << "Parse error: " << e.what() << "\n";
  }
}

}  // namespace UniClipboard
