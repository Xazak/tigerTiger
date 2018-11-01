#include <math.h> // sqrtf()
#include "main.hpp"

Actor::Actor(int inputX, int inputY, int sigil, const TCODColor &color,
	const char *name):
	xpos(inputX), ypos(inputY), sigil(sigil), color(color), name(name),
	obstructs(true) {LOGMSG("Created player at " << xpos << ", " << ypos); }
Actor::~Actor() {
	// destructor
}
void Actor::update() {
	if (sentience) sentience->update(this);
}
void Actor::render() const {
	// draw the actor on the map
//	LOGMSG("Drawing " << this->name << " at " << xpos << ", " << ypos);
	engine.gui->viewport->setChar(xpos, ypos, sigil); // draw the character
	engine.gui->viewport->setCharForeground(xpos, ypos, color); // color the character
	TCODConsole::blit(engine.gui->viewport, 0, 0, engine.gui->viewport->getWidth(), engine.gui->viewport->getHeight(), TCODConsole::root, 0, 0);
}
float Actor::getDistance(int targetX, int targetY) const {
	// returns the distance between this actor and the specified coordinates
	int diffX = xpos - targetX;
	int diffY = ypos - targetY;
	return sqrtf(diffX * diffX + diffY * diffY);
}
