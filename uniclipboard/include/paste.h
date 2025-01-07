#pragma once
#include <string>
#include "netutil.h"

namespace UniClipboard {

inline void Paste() {
  auto res = NetPull("/list_all_files_and_items");
  if (!res) {
    return;
  }

  try {
    auto items = Json::parse(res->body);
    // todo: timestamp use unix format

    std::cout << items;
    auto size = items.size();
    if (size < 1) {
      return;
    }

    auto item = items[0];

    std::string type = item["type"];
    if (type == "file") {
      std::string url = item["_id"];
      url = "/download_file/" + url;
      std::string save_path = item["filename"];
      save_path = "./" + save_path;
      NetPullFile(save_path, url);

    } else {
      std::string text = item["content"];
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
    }

  } catch (const std::exception& e) {
    std::cerr << "Parse error: " << e.what() << "\n";
  }
}
}  // namespace UniClipboard
