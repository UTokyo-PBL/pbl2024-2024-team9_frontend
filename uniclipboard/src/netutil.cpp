#include <iostream>
#include <ostream>
#include <string>
#include "httplib.h"
#include "json.hpp"

using Json = nlohmann::json;

auto const REMOTE = "http://57.180.86.229";

// todo: encrpyt
void Net_Send(Json& json, char* url) {
  auto content = json.dump();

  httplib::Client cli(REMOTE);
  cli.set_connection_timeout(5);

  try {
    auto res = cli.Post(url, content, "application/json");
    if (res == nullptr) {
      printf("failed\n");
      return;
    }

    std::cout << "status:" << res->status << std::endl;
    std::cout << "Response: " << res->body << std::endl;
  } catch (const std::exception& e) {
    std::cout << e.what();
  }
}

void TestHttp() {
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
