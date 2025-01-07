#pragma once
#include <string>
#include "netutil.h"

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

inline void PasteToClipboard() {
  Json json = {};
  auto res = NetPull(json, "/list_all_items");
  if (!res) {
    return;
  }

  try {
    auto items = Json::parse(res->body);
    std::cout << res->body << std::endl;
    auto size = items.size();
    if (size < 1) {
      return;
    }

    std::string text = items[0]["content"];

    // Open the clipboard
    if (OpenClipboard(NULL)) {
      // Empty the clipboard
      EmptyClipboard();

      // Allocate memory for the clipboard data
      HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
      if (hMem == NULL) {
        std::cerr << "Failed to allocate memory for clipboard!" << std::endl;
        CloseClipboard();
        return;
      }

      // Copy the text into the allocated memory
      char* pMem = static_cast<char*>(GlobalLock(hMem));
      if (pMem != NULL) {
        strcpy_s(pMem, text.size() + 1, text.c_str());
        GlobalUnlock(hMem);

        // Set the clipboard data (text format)
        SetClipboardData(CF_TEXT, hMem);
      }

      // Close the clipboard
      CloseClipboard();
    }

  } catch (const std::exception& e) {
    std::cerr << "Parse error: " << e.what() << "\n";
  }
}

}  // namespace UniClipboard
