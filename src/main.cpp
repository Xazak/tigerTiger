#include "main.hpp"

Engine engine(80, 50);

int main() {
// outermost game loop: runs until player closes game window
while ( !TCODConsole::isWindowClosed() ) {
    engine.update();
    engine.render();
    TCODConsole::flush();
    }
return 0;
}

