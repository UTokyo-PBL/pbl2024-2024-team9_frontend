#pragma once
#include <string>
#include "netutil.h"

inline void CopyText(std::string& text) {
  Json json = {
      {"content", text.c_str()},
  };

  auto res = NetSend(json, "/add_item");
  if (res) {
    std::cout << res->body;
  }
}
