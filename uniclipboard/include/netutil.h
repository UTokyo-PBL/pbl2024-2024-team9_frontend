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
      token = "Bearer " + token;
      return token;
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

inline httplib::Result NetSendFile(const std::string& path, const std::string& url) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    std::cerr << "Failed to open file!" << std::endl;
    return httplib::Result();
  }

  std::string file_data((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

  httplib::Client cli(REMOTE);
  cli.set_connection_timeout(5);

  try {
    httplib::Headers headers = {{"Authorization", GetToken().c_str()}};

    std::string filename = std::filesystem::path(path).filename().string();
    httplib::MultipartFormDataItems items;
    items.push_back(httplib::MultipartFormData{"file", file_data, filename,
                                               "application/octet-stream"});

    auto res = cli.Post(url.c_str(), headers, items);
    if (res == nullptr) {
      std::cerr << "NetSend failed" << std::endl;
      return httplib::Result();
    }

    std::cout << res->body << std::endl;
    return res;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return httplib::Result();
  }
}

inline httplib::Result NetPull(const std::string& url) {
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

inline bool NetPullFile(const std::string& save_path, const std::string& down_url) {
  std::string url = down_url;

  std::regex url_regex(R"(https://([^/]+)(/.*)?)");
  std::smatch url_match;
  if (!std::regex_match(url, url_match, url_regex)) {
    std::cout << "Invalid or unsupported url: " << url << std::endl;
    return false;
  }

  std::string host = url_match[1];
  std::string path = url_match[2];
  if (path.empty()) {
    path = "/";
  }

  httplib::Client cli(host.c_str());
  cli.set_connection_timeout(10);
  cli.set_follow_location(true);

  try {
    // httplib::Headers headers = {{"Authorization", GetToken().c_str()}};

    std::cout << "downfile url:" + url << std::endl;

    auto res = cli.Get(path.c_str());
    if (res == nullptr) {
      std::cout << "NetPullFile failed" << std::endl;
      return false;
    }

    if (res->status != 200) {
      std::cout << "Failed to download file, status: " << res->status;
      std::cout << "body: " << res->body << std::endl;
      return false;
    }

    std::ofstream output_file(save_path, std::ios::binary);
    if (!output_file.is_open()) {
      std::cout << "Failed to open file for saving at path: " << save_path << std::endl;
      return false;
    }

    output_file.write(res->body.c_str(), res->body.size());
    if (!output_file.good()) {
      std::cout << "Error occurred while writing to file: " << save_path << std::endl;
      return false;
    }

    return true;
  } catch (const std::exception& e) {
    std::cout << "Exception: " << e.what() << std::endl;
    return false;
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
