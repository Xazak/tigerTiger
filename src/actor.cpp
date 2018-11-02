#include <math.h> // sqrtf()
#include "main.hpp"

Actor::Actor(int inputX, int inputY, int sigil, const TCODColor &color,
	const char *name):
	xpos(inputX), ypos(inputY), sigil(sigil), color(color), name(name),
	obstructs(true) {LOGMSG("Created " << name << " at " << xpos << ", " << ypos); }
Actor::~Actor() {
	// destructor
}
void Actor::update() {
	if (sentience) sentience->update(this);
}
void Actor::render() const {
	// draw the actor on the map
//	LOGMSG("Drawing " << this->name << " at " << xpos << ", " << ypos);
	// the camera's position is determined by the center of the viewport
	int cameraXPos = engine.gui->viewport->getWidth() / 2;
	int cameraYPos = engine.gui->viewport->getHeight() / 2;
	// the target_Pos controls where the actor is actually drawn
	int targetXPos = 0;
	int targetYPos = 0;
	// make adjustments according to viewport:map position relationship
	// if the viewport's at the map edge, allow the player glyph to slide
	// if the viewport has room to move, the player should be at the camera_Pos
	if (engine.player->xpos < cameraXPos) {
		targetXPos = engine.player->xpos;
	} else {
		targetXPos = cameraXPos;
	}
	if (engine.player->ypos < cameraYPos) {
		targetYPos = engine.player->ypos;
	} else {
		targetYPos = cameraYPos;
	}
	// draw the actor itself
	engine.gui->viewport->setChar(targetXPos, targetYPos, sigil); // draw the character
	engine.gui->viewport->setCharForeground(targetXPos, targetYPos, color); // color the character
}
float Actor::getDistance(int targetX, int targetY) const {
	// returns the distance between this actor and the specified coordinates
	int diffX = xpos - targetX;
	int diffY = ypos - targetY;
	return sqrtf(diffX * diffX + diffY * diffY);
}
