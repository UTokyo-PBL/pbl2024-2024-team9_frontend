#ifdef MACOS_BUILD
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "daemoner.h"

void daemonize() {
  pid_t pid = fork();

  if (pid < 0) {
    std::cerr << "Fork failed!" << std::endl;
    exit(1);
  }

  if (pid > 0) {
    // Exit the parent process
    exit(0);
  }

  // Create a new session
  if (setsid() < 0) {
    std::cerr << "Failed to create a new session!" << std::endl;
    exit(1);
  }

  // Change the current working directory to root
  if (chdir("/") < 0) {
    std::cerr << "Failed to change working directory!" << std::endl;
    exit(1);
  }

  // Redirect standard file descriptors
  freopen("/dev/null", "r", stdin);   // Standard input
  freopen("/dev/null", "w", stdout);  // Standard output
  freopen("/dev/null", "w", stderr);  // Standard error

  // Main loop of the daemon
  while (true) {
    // Daemon logic here (e.g., performing tasks)
    std::cout << "Daemon is running..." << std::endl;
    sleep(10);  // Simulate doing some work
  }
}

#endif  // DEBUG
