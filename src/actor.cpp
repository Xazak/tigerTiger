#include <math.h> // sqrtf()
#include "main.hpp"

Actor::Actor(int inputX, int inputY, int sigil, const TCODColor &color,
	const char *name):
	xpos(inputX), ypos(inputY), sigil(sigil), color(color), name(name),
	obstructs(true) {
		LOGMSG("Created " << name << " at " << xpos << ", " << ypos);
	}
Actor::~Actor() {
	// destructor
}
void Actor::update() {
	if (sentience) sentience->update(this);
}
void Actor::render() const {
	// draw the actor on the map
	// find offsets for the viewport size
	// the camera's centerpoint will always be located at the viewport middle
	int viewportXOffset = engine.gui->viewport->getWidth() / 2;
	int viewportYOffset = engine.gui->viewport->getHeight() / 2;
	int screenXPos = 0; // x-position of actor on screen relative by map
	int screenYPos = 0; // y-position of actor on screen relative by map
	// determine where to draw the actor by the map:viewport relationship
	if (this == engine.player) {
		// lock the player position to the viewport middle unless they're too
		// close to a map edge, and then allow them to slide
		// edge checks, in order: left, right, top, bottom
		if (xpos < viewportXOffset) {
			screenXPos = xpos;
		} else if (xpos > (engine.map->width - viewportXOffset)) {
			screenXPos = engine.gui->viewport->getWidth() - (engine.map->width - engine.player->xpos);
		} else {
			screenXPos = viewportXOffset;
		}
		if (ypos < viewportYOffset) {
			screenYPos = ypos;
		} else if (ypos > (engine.map->height - viewportYOffset)) {
			screenYPos = engine.gui->viewport->getHeight() - (engine.map->height - engine.player->ypos);
		} else {
			screenYPos = viewportYOffset;
		}
		LOGMSG("Offset " << viewportXOffset << " by " << (xpos % viewportXOffset));
		LOGMSG("PLAYER abs: " << xpos << ", " << ypos << " rel: " << screenXPos << ", " << screenYPos);
	} else {
		// figure out where to draw the NPC actor by calculating the offset
		if (xpos > viewportXOffset) screenXPos = xpos % viewportXOffset;
		if (ypos > viewportYOffset) screenYPos = ypos % viewportYOffset;
	}

/*		if (xpos < cameraXPos || xpos > () {
			screenXPos = xpos;
		} else {
			screenXPos = cameraXPos;
		}
		if (ypos < cameraYPos) {
			screenYPos = ypos;
		} else {
			screenYPos = cameraYPos;
		}*/
	// draw the actor itself
	engine.gui->viewport->setChar(screenXPos, screenYPos, sigil); // draw the character
	engine.gui->viewport->setCharForeground(screenXPos, screenYPos, color); // color the character
//	LOGMSG("Drawing " << name << " at " << xpos << ", " << ypos << " [" << screenXPos << ", " << screenYPos << "]");
}
float Actor::getDistance(int targetX, int targetY) const {
	// returns the distance between this actor and the specified coordinates
	int diffX = xpos - targetX;
	int diffY = ypos - targetY;
	return sqrtf(diffX * diffX + diffY * diffY);
}
