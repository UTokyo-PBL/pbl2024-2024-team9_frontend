#pragma once
#include <string>
#include "netutil.h"
#include "json.hpp"

namespace UniClipboard {

using Json = nlohmann::json;

inline void UserLogin(std::string& uname, std::string& pwd) {
  Json json = {
      {"username", uname.c_str()},
      // todo: encrypt
      {"password", pwd.c_str()},
  };

  auto res = UniClipboard::NetSend(json, "/login");
  if (res) {
    try {
      auto resjson = Json::parse(res->body);
      std::string token = resjson["token"];
      if (token.empty()) {
        std::cerr << "Empty Token" << std::endl;
        return;
      }
      auto path = UniClipboard::TokenPath();
      std::ofstream outFile(path);

      if (!outFile) {
        std::cerr << "Failed create token file" << path << std::endl;
        return;
      }

      outFile << token;
      outFile.close();
      return;

    } catch (const std::exception& e) {
      std::cerr << "Parse error: " << e.what() << "\n";
    }
    std::cout << res->body;
  }
}
}  // namespace UniClipboard
