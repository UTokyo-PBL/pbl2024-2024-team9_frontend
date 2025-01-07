#include "gui.h"
#include <string>
#include "user.h"
#include <windef.h>
#include <windows.h>

namespace UniClipboard {

// Function to handle the login button click
void onLoginButtonClick(HWND hwndUsername, HWND hwndPassword, HWND hwnd) {
  char username[256];
  char password[256];

  GetWindowText(hwndUsername, username, sizeof(username));
  GetWindowText(hwndPassword, password, sizeof(password));

  std::string uname = username;
  std::string pwd = password;
  UniClipboard::UserLogin(uname, pwd);
  PostMessage(hwnd, WM_CLOSE, 0, 0);
}

// Window procedure function
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  static HWND hwndUsername, hwndPassword, hwndLoginButton;
  switch (msg) {
    case WM_CREATE:
      // username label and input
      CreateWindow("STATIC", "Username:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd,
                   NULL, NULL, NULL);
      hwndUsername = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20,
                                  150, 20, hwnd, NULL, NULL, NULL);

      CreateWindow("STATIC", "Password:", WS_VISIBLE | WS_CHILD, 20, 60, 100, 20, hwnd,
                   NULL, NULL, NULL);
      hwndPassword =
          CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 120,
                       60, 150, 20, hwnd, NULL, NULL, NULL);

      // login button
      hwndLoginButton = CreateWindow("BUTTON", "Login", WS_VISIBLE | WS_CHILD, 120, 100,
                                     150, 30, hwnd, (HMENU)1, NULL, NULL);

      break;

    case WM_COMMAND:
      if (LOWORD(wParam) == 1) {
        onLoginButtonClick(hwndUsername, hwndPassword, hwnd);
      }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

void GUILogin() {
  const char CLASS_NAME[] = "LoginDialogClass";
  WNDCLASS wc = {};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = CLASS_NAME;
  RegisterClass(&wc);

  int windowWidth = 300;
  int windowHeight = 200;

  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);

  // start in center of desktop
  int xPos = (screenWidth - windowWidth) / 2;
  int yPos = (screenHeight - windowHeight) / 2;

  HWND hwnd = CreateWindowEx(0, CLASS_NAME, "UniClipboard", WS_OVERLAPPEDWINDOW, xPos, yPos,
                             windowWidth, windowHeight, NULL, NULL, wc.hInstance, NULL);

  if (hwnd == NULL) {
    return;
  }

  ShowWindow(hwnd, SW_SHOWNORMAL);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return;
}

}  // namespace UniClipboard
