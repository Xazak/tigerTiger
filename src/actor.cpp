/*	 actor.cpp
DATE Oct 08 2018
AUTH xazak
DESC Implementation of Actor class and associated subclasses
 */
#include "main.hpp"
#include <string.h> // strdup
#include <math.h> // sqrtf()

Actor::Actor(int inputX, int inputY, int sigil, const TCODColor &color,
	const char *newName):
	xpos(inputX), ypos(inputY),
	sigil(sigil),
	obstructs(true), color(color),
	sentience(nullptr), vitality(nullptr), mortality(nullptr), //violent(nullptr),
	tempo(nullptr),	container(nullptr), portable(nullptr)
	{//		LOGMSG("Created " << name << " at " << xpos << ", " << ypos);	
		if (newName == nullptr) {
			name = "nobody";
		} else {
			name = strdup(newName);
		}
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
	// this should really go farther down, in the sentience logic maybe...
//	this->tempo->setMinimumAPCost(this->sentience->getCheapestActionCost());
	// are we asking to update a creature?
	if (sentience) {
		stateChange = sentience->update(this);
	}
	// or perhaps an item? (ie invoking a tool)
	// if they have health and therefore satiety, cycle it and do bookkeeping
	if (vitality) {
//		this->vitality->metabolize();
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
	fileBuffer.putString(name);
	fileBuffer.putInt(obstructs);
	fileBuffer.putColor(&color);
	// these are booleans that confirm whether this actor has these components
	// if the actor has these components, save their data as well

	LOGMSG("saving sentience: " << (sentience != nullptr));
	fileBuffer.putInt(sentience != nullptr);
	if (sentience) sentience->save(fileBuffer);

	LOGMSG("saving vitality: " << (vitality != nullptr));
	fileBuffer.putInt(vitality != nullptr);
	if (vitality) vitality->save(fileBuffer);

	LOGMSG("saving mortality: " << (mortality != nullptr));
	fileBuffer.putInt(mortality != nullptr);
	if (mortality) mortality->save(fileBuffer);

	LOGMSG("saving tempo: " << (tempo != nullptr));
	fileBuffer.putInt(tempo != nullptr);
	if (tempo) tempo->save(fileBuffer);

	LOGMSG("saving container: " << (container != nullptr));
	fileBuffer.putInt(container != nullptr);
	if (container) container->save(fileBuffer);

	LOGMSG("saving portable: " << (portable != nullptr));
	fileBuffer.putInt(portable != nullptr);
	// portable does not require persistence
	
//	LOGMSG("saving attacker: " << (attacker != nullptr));
//	fileBuffer.putInt(attacker != NULL);
//	if (attacker) attacker->save(fileBuffer);
}
void Actor::load(TCODZip &fileBuffer, bool isPlayer) {
//	LOGMSG("called");
	xpos = fileBuffer.getInt();
//	LOGMSG("xpos: " << xpos);
	ypos = fileBuffer.getInt();
//	LOGMSG("ypos: " << ypos);
	sigil = fileBuffer.getInt();
//	LOGMSG("sigil: " << ((char)sigil));
//	if (!name) LOGMSG("no existing name array on actor!");
	name = strdup(fileBuffer.getString());
//	LOGMSG("name: " << name);
	obstructs = fileBuffer.getInt();
//	LOGMSG("obstructs: " << obstructs);
	color = fileBuffer.getColor();
//	LOGMSG("color: " << color);
//	bool canAttack = fileBuffer.getInt();
//	LOGMSG("canAttack: " << canAttack);
	bool hasSentience = fileBuffer.getInt();
	LOGMSG("sentience: " << hasSentience);
	if (hasSentience) {
		if (isPlayer) {
			LOGMSG("actor given Player AI");
//			sentience = new PlayerSentience(fileBuffer);
			sentience = new PlayerSentience();
			sentience->load(fileBuffer);
		} else {
			LOGMSG("actor given Animal AI");
//			sentience = new AnimalSentience(fileBuffer);
			sentience = new AnimalSentience();
			sentience->load(fileBuffer);
		}
	}
	bool hasVitality = fileBuffer.getInt();
	LOGMSG("vitality: " << hasVitality);
	if (hasVitality) {
		vitality = new Vitality();
		vitality->load(fileBuffer);
	}
	bool hasMortality = fileBuffer.getInt();
	LOGMSG("mortality: " << hasMortality);
	if (hasMortality) {
		mortality = new Mortality(0, 0, nullptr);
		mortality->load(fileBuffer);
	}
	bool hasTempo = fileBuffer.getInt();
	LOGMSG("tempo: " << hasTempo);
	if (hasTempo) {
		tempo = new ActorClock(0);
		tempo->load(fileBuffer);
	}
	bool hasContainer = fileBuffer.getInt();
	LOGMSG("container: " << hasContainer);
	if (hasContainer) {
		container = new Container(0);
		container->load(fileBuffer);
	}
	bool hasPortable = fileBuffer.getInt();
	LOGMSG("portable: " << hasPortable);
	if (hasPortable) {
		portable = new Portable();
		// does not have any data to load
	}
}
