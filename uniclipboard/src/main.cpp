#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include "httplib.h"

void test_http() {
  // HTTP
  httplib::Client cli("34.226.255.154:80");

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
    sleep(3);
  }
}

int main(int argc, char* argv[]) {
  printf("starting...\n");
  test_http();
}
