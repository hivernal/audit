#include <iostream>

#include "audit/mainwindow.h"

int main() try {
  audit::MainWindow app{};
  app.Run();
  return 0;
} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
  return 1;
}
