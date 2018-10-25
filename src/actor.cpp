#include <math.h> // sqrtf()
#include "main.hpp"

Actor::Actor(int inputX, int inputY, int sigil, const TCODColor &color, const char *name):
	actorX(inputX), actorY(inputY), sigil(sigil), color(color), name(name),
	obstructs(true) { }
Actor::~Actor() {
	// destructor
}
void Actor::update() {
	// update this actor by their attached AI script
}
void Actor::render() const {
	// draw the actor on the map
	TCODConsole::root->setChar(actorX, actorY, sigil); // draw the character
	TCODConsole::root->setCharForeground(actorX, actorY, color); // color the character
}
float Actor::getDistance(int targetX, int targetY) const {
	// returns the distance between this actor and the specified coordinates
	int diffX = actorX - targetX;
	int diffY = actorY - targetY;
	return sqrtf(diffX * diffX + diffY * diffY);
}
