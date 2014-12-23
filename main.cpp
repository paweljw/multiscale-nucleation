#include "globals.h"
#include "MultiscaleWidget.h"

// Look in MultiscaleWidget.cpp for Qt-related stuff,
// in MonteCarloProcessor.cpp for MC-related stuff,
// in NucleationProcessor.cpp for nucleation-related stuff,
// in globals.h for config,
// in node.h for Node class.

// Other files are generated.

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);  
    
  MultiscaleWidget window;

  window.resize(WIDTH + 200, HEIGHT + 20);
  window.setWindowTitle("Pawel J. Wal - Nucleations");
  window.show();

  return app.exec();
}