#pragma once
#include <string>
#include "netutil.h"
#include "json.hpp"
using Json = nlohmann::json;

inline void UserLogin(std::string& uname, std::string& pwd) {
  Json json = {
      {"username", uname.c_str()},
      // todo: encrypt
      {"password", pwd.c_str()},
  };

  auto res = NetSend(json, "/login");
  if (res) {
    try {
      auto resjson = Json::parse(res->body);
      std::string token = resjson["token"];
      // todo: store token
      std::cout << "token is: " << token;
    } catch (const std::exception& e) {
      std::cerr << "Parse error: " << e.what() << "\n";
    }
    std::cout << res->body;
  }
}
