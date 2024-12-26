#pragma once
#include <iostream>
#include <ostream>
#include <string>
#include "httplib.h"
#include "json.hpp"

using Json = nlohmann::json;

auto const REMOTE = "http://57.180.86.229";

// todo: encrpyt
inline httplib::Result NetSend(Json& json, const std::string& url) {
  auto content = json.dump();

  std::cout << content + "\n";

  httplib::Client cli(REMOTE);
  cli.set_connection_timeout(5);

  try {
    httplib::Headers headers = {
        {"Authorization",
         "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9."
         "eyJ1c2VyX2lkIjoiNjc2NThmODVlZmY3M2ZkODE2YzBjMjdkIiwiZXhwIjoxNzM1MjQwMjY3fQ."
         "WTymwYQi5L2iyVBZyHPI2UkVxzMlX0E96xfqLkUgj9o"},
    };

    // todo: token
    auto res = cli.Post(url, headers, content, "application/json");
    if (res == nullptr) {
      std::cerr << "Net_send failed";
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
