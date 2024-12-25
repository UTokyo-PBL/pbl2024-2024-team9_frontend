#pragma once
#include <string>
#include "httplib.h"
#include "json.hpp"

void TestHttp();

httplib::Result Net_Send(nlohmann::json& json, const std::string& url);
