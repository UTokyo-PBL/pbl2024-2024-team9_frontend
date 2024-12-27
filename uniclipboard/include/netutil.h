#pragma once
#include <iostream>
#include <ostream>
#include <string>
#include "httplib.h"
#include "json.hpp"

namespace UniClipboard {

using Json = nlohmann::json;

auto const REMOTE = "http://57.180.86.229";

inline std::string TokenPath() {
  // todo: put it in userpath
  return "./token.txt";
}

inline std::string GetToken() {
  static std::string token = "";
  if (token != "") {
    return token;
  }

  std::ifstream file(TokenPath());
  if (file.is_open()) {
    std::ostringstream buffer;
    buffer << file.rdbuf();
    token = buffer.str();
    file.close();
    if (token != "") {
      return "Bearer " + token;
    }
  }

  return "";
}

// todo: encrpyt
inline httplib::Result NetSend(Json& json, const std::string& url) {
  auto content = json.dump();

  httplib::Client cli(REMOTE);
  cli.set_connection_timeout(5);

  try {
    httplib::Headers headers = {{"Authorization", GetToken().c_str()}};

    auto res = cli.Post(url, headers, content, "application/json");
    if (res == nullptr) {
      std::cerr << "NetSend failed";
      return httplib::Result();
    }

    return res;
  } catch (const std::exception& e) {
    std::cout << e.what();
    return httplib::Result();
  }
}

inline httplib::Result NetPull(Json& json, const std::string& url) {
  auto content = json.dump();

  httplib::Client cli(REMOTE);
  cli.set_connection_timeout(5);

  try {
    httplib::Headers headers = {{"Authorization", GetToken().c_str()}};

    auto res = cli.Get(url, headers);
    if (res == nullptr) {
      std::cerr << "NetPull failed";
      return httplib::Result();
    }

    return res;
  } catch (const std::exception& e) {
    std::cout << e.what();
    return httplib::Result();
  }
}

inline void TestHttp() {
  // HTTP
  httplib::Client cli(REMOTE);

  // HTTPS
  // httplib::Client cli("https://cpp-httplib-server.yhirose.repl.co");

  cli.set_connection_timeout(5);
  while (1) {
    printf("access server \n");
    try {
      auto res = cli.Get("/index.html");
      if (res == nullptr) {
        printf("failed\n");
        continue;
      }
      std::cout << "status:" << res->status << std::endl;
      // std::cout << "body" << res->body << std::endl;
    } catch (const std::exception& e) {
      std::cout << e.what();
    }
  }
}
}  // namespace UniClipboard
