#include "globals.h"
#include "MultiscaleWidget.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);  
    
  MultiscaleWidget window;

  window.resize(WIDTH + 200, HEIGHT + 20);
  window.setWindowTitle("Pawel J. Wal - Nucleations");
  window.show();

  return app.exec();
}