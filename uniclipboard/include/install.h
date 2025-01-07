#pragma once
#include <iostream>

namespace UniClipboard {

bool AddToContextMenu(const std::string& programPath);
bool RemoveFromContextMenu(const std::string& programPath);
int install(int option, std::string prog_path);

}  // namespace UniClipboard
