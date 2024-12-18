#include <windows.h>
#include <iostream>
#include <string>

auto const PNAME = "UniClipboardCopy";

bool AddToContextMenu(const std::string& programPath) {
  HKEY hKey;
  LONG result;

  const std::string shellPath = R"(SOFTWARE\Classes\*\shell\)" + std::string(PNAME);
  const std::string commandPath = shellPath + "\\command";

  // Create the shell key
  result = RegCreateKeyEx(HKEY_CURRENT_USER, shellPath.c_str(), 0, nullptr,
                          REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);

  if (result != ERROR_SUCCESS) {
    std::cerr << "Failed to create registry key '" << shellPath << "'. Error: " << result
              << "\n";
    return false;
  }

  // Set the display name
  const std::string displayName = "Copy With UniClipboard";
  result = RegSetValueEx(hKey, nullptr, 0, REG_SZ,
                         reinterpret_cast<const BYTE*>(displayName.c_str()),
                         (displayName.size() + 1) * sizeof(char));
  if (result != ERROR_SUCCESS) {
    std::cerr << "Failed to set display name. Error: " << result << "\n";
    RegCloseKey(hKey);  // Close the key on error
    return false;
  }

  // Add the NoOpen value to prevent console window
  DWORD noOpenValue = 1;
  result = RegSetValueEx(hKey, "NoOpen", 0, REG_DWORD,
                         reinterpret_cast<const BYTE*>(&noOpenValue), sizeof(DWORD));
  RegCloseKey(hKey);  // Close the shell key here

  if (result != ERROR_SUCCESS) {
    std::cerr << "Failed to set NoOpen value. Error: " << result << "\n";
    return false;
  }

  // Create the command key
  result = RegCreateKeyEx(HKEY_CURRENT_USER, commandPath.c_str(), 0, nullptr,
                          REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);

  if (result != ERROR_SUCCESS) {
    std::cerr << "Failed to create command key '" << commandPath << "'. Error: " << result
              << "\n";
    return false;
  }

  // Set the program path with %1 for the selected file
  const std::string command = "\"" + programPath + "\" \"%1\"";
  result = RegSetValueEx(hKey, nullptr, 0, REG_SZ,
                         reinterpret_cast<const BYTE*>(command.c_str()),
                         (command.size() + 1) * sizeof(char));
  RegCloseKey(hKey);
  if (result != ERROR_SUCCESS) {
    std::cerr << "Failed to set command value. Error: " << result << "\n";
    return false;
  }

  return true;
}

bool RemoveFromContextMenu(const std::string& programPath) {
  LONG result;

  auto shellPath = std::string(R"(SOFTWARE\Classes\*\shell\)") + PNAME;
  auto commandPath = shellPath + "\\command";

  // Delete the command key (associated with the program execution)
  result = RegDeleteKey(HKEY_CURRENT_USER, commandPath.c_str());
  if (result != ERROR_SUCCESS) {
    std::cerr << "Failed to delete command key. Error: " << result << "\n";
    return false;
  }

  // Delete the shell key (the context menu entry itself)
  result = RegDeleteKey(HKEY_CURRENT_USER, shellPath.c_str());
  if (result != ERROR_SUCCESS) {
    std::cerr << "Failed to delete shell key. Error: " << result << "\n";
    return false;
  }

  return true;
}
