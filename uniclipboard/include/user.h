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

  Net_Send(json, "/login");
}
