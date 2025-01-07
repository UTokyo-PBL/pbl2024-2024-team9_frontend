#pragma once
#include <string>
#include "netutil.h"
#include <windows.h>

namespace UniClipboard {

inline std::string GetClipboardText() {
  std::string text = "";

  if (!OpenClipboard(nullptr))
    return "";

  HANDLE hData = GetClipboardData(CF_TEXT);
  if (hData == nullptr) {
    CloseClipboard();
    return "";
  }

  // lock handle to get text
  char* pszText = static_cast<char*>(GlobalLock(hData));
  if (pszText == nullptr) {
    CloseClipboard();
    return "";
  }

  text = pszText;

  GlobalUnlock(hData);

  CloseClipboard();

  return text;
}

inline void CopyClipboardText() {
  auto text = GetClipboardText();
  if (text.empty()) {
    return;
  }

  std::cout << text << std::endl;

  Json json = {
      {"content", text.c_str()},
  };

  auto res = NetSend(json, "/add_item");
  if (res) {
    std::cout << res->body << std::endl;
  }
}

inline void CopyFromFile(const std::string& path) {
  NetSendFile(path, "/upload_file");
  return;
}

}  // namespace UniClipboard
