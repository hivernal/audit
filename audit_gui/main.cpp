#include <iostream>

#include "audit_gui/mainwindow.h"

int main() try {
  MainWindow app{};
  app.Run();
  return 0;
} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
  return 1;
}
