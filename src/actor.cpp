/*	 actor.cpp
DATE Oct 08 2018
AUTH xazak
DESC Implementation of Actor class and associated subclasses
 */
#include <math.h> // sqrtf()
#include "main.hpp"

Actor::Actor(int inputX, int inputY, int sigil, const TCODColor &color,
	const char *name):
	xpos(inputX), ypos(inputY), sigil(sigil), color(color), name(name),
	obstructs(true) {
//		LOGMSG("Created " << name << " at " << xpos << ", " << ypos);
	}
Actor::Actor(TCODZip &fileBuffer, bool isPlayer) {
	LOGMSG("called");
	xpos = fileBuffer.getInt();
	LOGMSG("xpos: " << xpos);
	ypos = fileBuffer.getInt();
	LOGMSG("ypos: " << ypos);
	sigil = fileBuffer.getInt();
	LOGMSG("sigil: " << ((char)sigil));
	color = fileBuffer.getColor();
//	LOGMSG("color: " << color);
	if (!name) LOGMSG("no existing name array on actor!");
	if (strdup(fileBuffer.getString())) ERRMSG("NO NAME FOUND");
//	name = strdup(fileBuffer.getString());
//	LOGMSG("name: " << name);
	obstructs = fileBuffer.getInt();
	LOGMSG("obstructs: " << obstructs);
//	bool canAttack = fileBuffer.getInt();
//	LOGMSG("canAttack: " << canAttack);
	bool canThink = fileBuffer.getInt();
	LOGMSG("canThink: " << canThink);
	bool canDie = fileBuffer.getInt();
	LOGMSG("canDie: " << canDie);
	bool canMove = fileBuffer.getInt();
	LOGMSG("canMove: " << canMove);
	bool canContain = fileBuffer.getInt();
	LOGMSG("canContain: " << canContain);
	bool isPortable = fileBuffer.getInt();
	LOGMSG("isPortable: " << isPortable);
	// is there an attacker module?
//	if (canAttack) attacker = new Attacker(fileBuffer);
	if (canThink) {
		if (isPlayer) {
			sentience = new PlayerSentience(fileBuffer);
			LOGMSG("actor given Player AI");
		} else {
			sentience = new AnimalSentience(fileBuffer);
			LOGMSG("actor given Animal AI");
		}
	}
	if (canDie) mortality = new Mortality(fileBuffer);
	if (canMove) tempo = new ActorClock(fileBuffer);
	if (canContain) container = new Container(fileBuffer);
	if (isPortable) portable = new Portable();
}
Actor::~Actor() {
	// destructor
}
bool Actor::update() {
	bool stateChange = false;
//	this->tempo->setMinimumAPCost(this->sentience->getCheapestActionCost());
	if (sentience) {
		stateChange = sentience->update(this);
	}
	return stateChange;
}
void Actor::render() const {
	// draw the actor on the map
	int screenXPos = 0; // x-position of actor on screen relative to map
	int screenYPos = 0; // y-position of actor on screen relative to map
	// determine where to draw the actor by the map:viewport relationship
	if (this == engine.player) {
		// lock the player position to the viewport middle unless they're too
		// close to a map edge, and then allow them to slide
		// left-right edge checks
		if (xpos < engine.gui->viewportXOffset) {
			screenXPos = xpos;
		} else if (xpos >= (engine.map->width - engine.gui->viewportXOffset)) {
			screenXPos = engine.gui->viewport->getWidth() - (engine.map->width - xpos);
		} else {
			screenXPos = engine.gui->viewportXOffset;
		}
		// top-bottom edge checks
		if (ypos < engine.gui->viewportYOffset) {
			screenYPos = ypos;
		} else if (ypos >= (engine.map->height - engine.gui->viewportYOffset)) {
			screenYPos = engine.gui->viewport->getHeight() - (engine.map->height - ypos);
		} else {
			screenYPos = engine.gui->viewportYOffset;
		}
//		LOGMSG("Offset " << viewportXOffset << " by " << (xpos % viewportXOffset));
//		LOGMSG("PLAYER abs: " << xpos << ", " << ypos << " rel: " << screenXPos << ", " << screenYPos);
	} else { // how to draw all the other NPCs
		screenXPos = xpos - engine.gui->viewportXOrigin;
		screenYPos = ypos - engine.gui->viewportYOrigin;
//		LOGMSG("Drawing " << name << " at " << xpos << ", " << ypos << " [" << screenXPos << ", " << screenYPos << "]");
	}
	// draw the actor itself
	engine.gui->viewport->setCharForeground(screenXPos, screenYPos, color); // color the character
	engine.gui->viewport->setChar(screenXPos, screenYPos, sigil); // draw the character
//	LOGMSG("Drawing " << name << " at " << xpos << ", " << ypos << " [" << screenXPos << ", " << screenYPos << "]");
}
float Actor::getDistance(int targetX, int targetY) const {
	// returns the distance between this actor and the specified coordinates
	int diffX = xpos - targetX;
	int diffY = ypos - targetY;
	return sqrtf(diffX * diffX + diffY * diffY);
}
void Actor::save(TCODZip &fileBuffer) {
	LOGMSG("called");
	fileBuffer.putInt(xpos);
	fileBuffer.putInt(ypos);
	fileBuffer.putInt(sigil);
	fileBuffer.putColor(&color);
	fileBuffer.putString(name);
	LOGMSG("saving name: " << name);
	fileBuffer.putInt(obstructs);
	// these are booleans that confirm whether this actor has these components
//	fileBuffer.putInt(attacker != NULL);
	fileBuffer.putInt(sentience != NULL);
	LOGMSG("saving sentience: " << (sentience != NULL));
	fileBuffer.putInt(mortality != NULL);
	LOGMSG("saving mortality: " << (mortality != NULL));
	fileBuffer.putInt(tempo != NULL);
	LOGMSG("saving tempo: " << (tempo != NULL));
	fileBuffer.putInt(container != NULL);
	LOGMSG("saving container: " << (container != NULL));
	fileBuffer.putInt(portable != NULL);
	LOGMSG("saving portable: " << (portable != NULL));
	// if the actor has these components, save their data as well
//	if (attacker) attacker->save(fileBuffer);
	if (sentience) sentience->save(fileBuffer);
	if (mortality) mortality->save(fileBuffer);
	if (tempo) {
		// are they in the middle of doing something already?
		fileBuffer.putInt(tempo->getCurrState() == ActorClock::ClockState::CHARGING);
		// we only need to save this data if we're charging an action
		if (tempo->getCurrState() == ActorClock::ClockState::CHARGING) {
			tempo->save(fileBuffer);
		}
	}
	if (container) container->save(fileBuffer);
	// portable does not require persistence
}
void Actor::load(TCODZip &fileBuffer, bool isPlayer) {
	LOGMSG("called");
	xpos = fileBuffer.getInt();
	LOGMSG("xpos: " << xpos);
	ypos = fileBuffer.getInt();
	LOGMSG("ypos: " << ypos);
	sigil = fileBuffer.getInt();
	LOGMSG("sigil: " << ((char)sigil));
	color = fileBuffer.getColor();
//	LOGMSG("color: " << color);
	if (!name) LOGMSG("no existing name array on actor!");
	name = strdup(fileBuffer.getString());
	LOGMSG("name: " << name);
	obstructs = fileBuffer.getInt();
	LOGMSG("obstructs: " << obstructs);
//	bool canAttack = fileBuffer.getInt();
//	LOGMSG("canAttack: " << canAttack);
	bool canThink = fileBuffer.getInt();
	LOGMSG("canThink: " << canThink);
	bool canDie = fileBuffer.getInt();
	LOGMSG("canDie: " << canDie);
	bool canMove = fileBuffer.getInt();
	LOGMSG("canMove: " << canMove);
	bool canContain = fileBuffer.getInt();
	LOGMSG("canContain: " << canContain);
	bool isPortable = fileBuffer.getInt();
	LOGMSG("isPortable: " << isPortable);
	// is there an attacker module?
//	if (canAttack) attacker = new Attacker(fileBuffer);
	if (canThink) {
		if (isPlayer) {
			sentience = new PlayerSentience(fileBuffer);
			LOGMSG("actor given Player AI");
		} else {
			sentience = new AnimalSentience(fileBuffer);
			LOGMSG("actor given Animal AI");
		}
	}
	if (canDie) mortality = new Mortality(fileBuffer);
	if (canMove) tempo = new ActorClock(fileBuffer);
	if (canContain) container = new Container(fileBuffer);
	if (isPortable) portable = new Portable();
}
