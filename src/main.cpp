#include "libtcod.hpp"
#include "actor.hpp"
#include "map.hpp"
#include "engine.hpp"

Engine engine;

int main() {
// outermost game loop: runs until player closes game window
while ( !TCODConsole::isWindowClosed() ) {
    engine.update();
    engine.render();
    TCODConsole::flush();
    }
return 0;
}

