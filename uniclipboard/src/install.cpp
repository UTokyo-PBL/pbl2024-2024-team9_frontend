#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

namespace UniClipboard {
auto const PNAME = "UniClipboardCopy";

struct ContextMenuCommand {
  std::string name;
  std::string displayName;
  std::string command;
};

bool AddToContextMenu(const std::string& programPath) {
  HKEY hKey;
  LONG result;

  std::vector<ContextMenuCommand> commands = {
      {"UniClipboardCopy", "Copy With UniClipboard",
       "\"" + programPath + "\" --copyfile \"%1\""},
      {"UniClipboardPaste", "Paste From UniClipboard",
       "\"" + programPath + "\" --pastefile \"%1\""}};

  for (const auto& cmd : commands) {
    const std::string shellPath = R"(SOFTWARE\Classes\*\shell\)" + cmd.name;

    result = RegCreateKeyEx(HKEY_CURRENT_USER, shellPath.c_str(), 0, nullptr,
                            REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);

    if (result != ERROR_SUCCESS) {
      std::cerr << "Failed to create/open registry key for command '" << cmd.name
                << "'. Error: " << result << "\n";
      return false;
    }

    // Set the display name for the command
    result = RegSetValueEx(hKey, nullptr, 0, REG_SZ,
                           reinterpret_cast<const BYTE*>(cmd.displayName.c_str()),
                           (cmd.displayName.size() + 1) * sizeof(char));
    if (result != ERROR_SUCCESS) {
      std::cerr << "Failed to set display name for command '" << cmd.name
                << "'. Error: " << result << "\n";
      RegCloseKey(hKey);
      return false;
    }

    // create the command subkey (i.e., \command)
    const std::string cmdCommandPath = shellPath + "\\command";
    HKEY hCmdKey;
    result =
        RegCreateKeyEx(HKEY_CURRENT_USER, cmdCommandPath.c_str(), 0, nullptr,
                       REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hCmdKey, nullptr);
    if (result != ERROR_SUCCESS) {
      std::cerr << "Failed to create command key for '" << cmdCommandPath
                << "'. Error: " << result << "\n";
      RegCloseKey(hKey);
      return false;
    }

    // set the command to be executed (e.g., programPath --file "%1")
    result = RegSetValueEx(hCmdKey, nullptr, 0, REG_SZ,
                           reinterpret_cast<const BYTE*>(cmd.command.c_str()),
                           (cmd.command.size() + 1) * sizeof(char));

    if (result != ERROR_SUCCESS) {
      std::cerr << "Failed to set command value for '" << cmd.name
                << "'. Error: " << result << "\n";
      RegCloseKey(hCmdKey);
      RegCloseKey(hKey);
      return false;
    }

    RegCloseKey(hCmdKey);
    RegCloseKey(hKey);
  }

  return true;
}

bool RemoveFromContextMenu(const std::string& programPath) {
  LONG result;

  std::vector<ContextMenuCommand> commands = {
      {"UniClipboardCopy", "Copy With UniClipboard", ""},
      {"UniClipboardPaste", "Paste From UniClipboard", ""}};

  for (const auto& cmd : commands) {
    const std::string cmdShellPath = R"(SOFTWARE\Classes\*\shell\)" + cmd.name;

    // delete the 'command' subkey first
    const std::string cmdCommandPath = cmdShellPath + "\\command";
    result = RegDeleteTree(HKEY_CURRENT_USER, cmdCommandPath.c_str());
    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
      std::cerr << "Failed to delete command subkey for '" << cmd.name
                << "'. Error: " << result << "\n";
    }

    // delete the main shell key
    result = RegDeleteTree(HKEY_CURRENT_USER, cmdShellPath.c_str());
    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
      std::cerr << "Failed to delete '" << cmd.name << "' key. Error: " << result << "\n";
    }
  }

  return true;
}

bool AddToDesktopContextMenu(const std::string& programPath) {
  HKEY hKey;
  LONG result;

  std::vector<ContextMenuCommand> commands = {
      {"UniClipboardCopy", "Copy from system clipboard",
       "\"" + programPath + "\" --copy"},
      {"UniClipboardPaste", "Paste to system clipboard",
       "\"" + programPath + "\" --paste"}};

  for (const auto& cmd : commands) {
    // register the context menu on the desktop (blank area)
    const std::string shellPath =
        R"(SOFTWARE\Classes\Directory\Background\shell\)" + cmd.name;

    result = RegCreateKeyEx(HKEY_CURRENT_USER, shellPath.c_str(), 0, nullptr,
                            REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);

    if (result != ERROR_SUCCESS) {
      std::cerr << "Failed to create/open registry key for command '" << cmd.name
                << "'. Error: " << result << "\n";
      return false;
    }

    result = RegSetValueEx(hKey, nullptr, 0, REG_SZ,
                           reinterpret_cast<const BYTE*>(cmd.displayName.c_str()),
                           (cmd.displayName.size() + 1) * sizeof(char));
    if (result != ERROR_SUCCESS) {
      std::cerr << "Failed to set display name for command '" << cmd.name
                << "'. Error: " << result << "\n";
      RegCloseKey(hKey);
      return false;
    }

    const std::string cmdCommandPath = shellPath + "\\command";
    HKEY hCmdKey;
    result =
        RegCreateKeyEx(HKEY_CURRENT_USER, cmdCommandPath.c_str(), 0, nullptr,
                       REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hCmdKey, nullptr);
    if (result != ERROR_SUCCESS) {
      std::cerr << "Failed to create command key for '" << cmdCommandPath
                << "'. Error: " << result << "\n";
      RegCloseKey(hKey);
      return false;
    }

    result = RegSetValueEx(hCmdKey, nullptr, 0, REG_SZ,
                           reinterpret_cast<const BYTE*>(cmd.command.c_str()),
                           (cmd.command.size() + 1) * sizeof(char));

    if (result != ERROR_SUCCESS) {
      std::cerr << "Failed to set command value for '" << cmd.name
                << "'. Error: " << result << "\n";
      RegCloseKey(hCmdKey);
      RegCloseKey(hKey);
      return false;
    }

    RegCloseKey(hCmdKey);
    RegCloseKey(hKey);
  }

  return true;
}

bool RemoveFromDesktopContextMenu(const std::string& programPath) {
  LONG result;

  std::vector<ContextMenuCommand> commands = {
      {"UniClipboardCopy", "Copy from system clipboard", ""},
      {"UniClipboardPaste", "Paste to system clipboard", ""}};

  for (const auto& cmd : commands) {
    // define the shell path for the desktop context menu
    const std::string cmdShellPath =
        R"(SOFTWARE\Classes\Directory\Background\shell\)" + cmd.name;

    // delete the 'command' subkey first
    const std::string cmdCommandPath = cmdShellPath + "\\command";
    result = RegDeleteTree(HKEY_CURRENT_USER, cmdCommandPath.c_str());
    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
      std::cerr << "Failed to delete command subkey for '" << cmd.name
                << "'. Error: " << result << "\n";
    }

    // delete the main shell key
    result = RegDeleteTree(HKEY_CURRENT_USER, cmdShellPath.c_str());
    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
      std::cerr << "Failed to delete '" << cmd.name << "' key. Error: " << result << "\n";
    }
  }

  return true;
}

int install(int option, std::string prog_path) {
  if (option == 1) {
    if (!AddToContextMenu(prog_path)) {
      return 1;
    }

    if (!AddToDesktopContextMenu(prog_path)) {
      return 1;
    }

  } else if (option == 0) {
    if (!RemoveFromContextMenu(prog_path)) {
      return 1;
    }
    if (!RemoveFromDesktopContextMenu(prog_path)) {
      return 1;
    }
  }

  return 0;
}

}  // namespace UniClipboard
