#pragma once
#include "json.hpp"

void TestHttp();

void Net_Send(nlohmann::json& json, char* url);
