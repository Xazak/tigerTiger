#include "libtcod.hpp"
#include "actor.hpp"

// x, y coords; actor glyph; glyph color
Actor::Actor(int x, int y, int ch, const TCODColor &col) :
   x(x),y(y),ch(ch),col(col) {
}

void Actor::render() const {
    // place the actor glyph on the screen
   TCODConsole::root->setChar(x,y,ch);
    // set the glyph color
   TCODConsole::root->setCharForeground(x,y,col);
}
