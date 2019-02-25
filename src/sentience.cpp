/*	 sentience.cpp
DATE Oct 24 2018
AUTH xazak
DESC Contains action packages that provide decision-making tools for Actors.
 */
#include "main.hpp"
#include <stdio.h>
#include <math.h>

// **** SENTIENCE
// **** utilities
void Sentience::save(TCODZip &fileBuffer) {
	LOGMSG("called");
	context->save(fileBuffer);
}
void Sentience::load(TCODZip &fileBuffer) {
	LOGMSG("called");
	context->load(fileBuffer);
}
// **** tools
bool Sentience::checkAction(ActionContext inputContext) {
	// checks to see if a given action can be performed successfully
	// if the action CANNOT be performed, it will trigger the correct feedback
	// 	including action conversions (?)
	// and return false
	// otherwise, it handles whatever preliminary feedback should occur
	// and returns true
	bool successFlag = true; // assume an action is valid until it isn't
	// only actions which require any preliminary checking will be listed
	// thus, the default case handles everything else
	int xTarget = 0;
	int yTarget = 0;
	switch (inputContext.currAction) {
		default:
			// ie WAIT,
			// do nothing
			break;
		case Sentience::Action::MOVE:
			// is the target tile obstructed?
			// get the absolute coords of the target tile
			xTarget = engine.player->xpos + inputContext.echs;
			yTarget = engine.player->ypos + inputContext.whye;
//			LOGMSG("player wants to move to " << xTarget << ", " << yTarget);
			if (engine.map->isObstructed(xTarget, yTarget)) {
				inputContext.setSuccess(false);
			}
			break;
	}
	return successFlag;
}
// **** diagnostics
/* compare strength
   check thirst
   check hunger
   check shelter
   check sleep
   check health
   */
int Sentience::compareStrength(Actor *first, Actor *second) {
	// if *second is not provided, defaults to self (calling actor)
	// returns a value indicating the relative difference in power level
	// between the two Actors, given the following:
	// PRECONDITIONS
	// -- the two actors are both visible (cannot target an unseen actor)
	// a negative value indicates advantage to the first; positive values
	// indicate an advantage to the second
	return 0;
}
uint Sentience::checkThirst(Actor *subject) {
	// returns a percentage value indicating water need:
	// 0 means "not thirsty at all", 100 means "dying of thirst"
	if (!subject->vitality) { // if the actor doesn't have vitality...
		return 0; // then the actor is never thirsty
	}
	return subject->vitality->getCurrentThirst();
}
uint Sentience::checkHunger(Actor *subject) {
	// returns a percentage value indicating food need:
	// 0 means "not hungry at all", 100 means "dying of hunger"
	if (!subject->vitality) { // if the actor doesn't have vitality...
		return 0; // then the actor is never hungry
	}
	return subject->vitality->getCurrentSatiety();
}
/*uint Sentience::checkShelter(Actor *subject) {
	return 0;
}*/
uint Sentience::checkSleep(Actor *subject) {
	// returns a percentage value indicating sleep need:
	// 0 means "not tired at all", 100 means "narcoleptic"
	if (!subject->vitality) { // if the actor doesn't have vitality...
		return 0; // then the actor is never sleepy
	}
	return subject->vitality->getCurrentSleep();
}
/*uint Sentience::checkHealth(Actor *subject) {
	return 0;
}*/
// **** universal verbs
void Sentience::wait(Actor *subject, int numOfTurns) {
	LOGMSG("called");
	//do nothing
}
void Sentience::moveRel(Actor *subject, int targetx, int targety) { // RELATIVE COORDS
	// move to the RELATIVE LOC specified by traveling on foot 'normally'
	// presumably we've already checked for a valid target location?
	// the subject is no longer occupying the current tile
	engine.map->setOccupant(subject->xpos, subject->ypos, nullptr);
	// move the subject to the new position
	subject->xpos += targetx;
	subject->ypos += targety;
	// make the subject the occupant of the new tile
	engine.map->setOccupant(subject->xpos, subject->ypos, subject);
}
void Sentience::moveAbs(Actor *subject, int targetx, int targety) { // ABSOLUTE COORDS
	// move to the ABSOLUTE LOC specified by traveling on foot 'normally'
	LOGMSG(" called !!");
	// ideally, this fxn should be working out some relative coords and then
	// passing over to moveRel(), such that the obstruction checks, etc can be
	// reused/stratified into a single location, in case they are changed later
}
//void Sentience::drop(Actor *subject, Actor *target);
//void Sentience::consume(Actor *subject, Actor *target); // change to only allow consumable actors?
//void Sentience::grab(Actor *subject, int targetx, int targety); // ABSOLUTE COORDS
//void Sentience::toggleSit(Actor *subject);
//void Sentience::rubOn(Actor *subject, Actor *target); // includes all general scent deposit verbs
void Sentience::groom(Actor *subject, Actor *target) {
	// social interaction, also allows self
	engine.gui->message(TCODColor::lightGrey, "%s reaches out and tugs on %s's tail!",
			subject->name,
			target->name );
}
// **** PLAYER Sentience
PlayerSentience::PlayerSentience() {
	context = new ActionContext();
}
PlayerSentience::PlayerSentience(TCODZip &fileBuffer) {
	context = new ActionContext();
	context->load(fileBuffer);
}
bool PlayerSentience::update(Actor *subject) {
	// by the time this function is invoked, we should know:
	// 1) Exactly what action is to be performed
	// 2) Any other details required for that action to be completed
//	LOGMSG(subject->name << " updating");
	// if the player's dead, don't even try to update
	if (subject->mortality && subject->mortality->isDead()) {
		ERRMSG(": Player is dead!");
		return false;
	}
	// FIRST THING: check to see if the world state will allow the action
	if (checkAction(*subject->sentience->context) == false) {
		// the player's action cannot be performed for some reason that
		// checkAction was able to determine
		// checkAction SHOULD NOT trigger feedback messages!
		// disclose error feedback to the player
		int xTarget = 0;
		int yTarget = 0;
		switch (context->currAction) {
			default:
				// do nothing
				break;
			case Sentience::Action::MOVE:
				// is the target tile obstructed?
				// get the absolute coords of the target tile
				xTarget = engine.player->xpos + context->echs;
				yTarget = engine.player->ypos + context->whye;
				if (engine.map->isWall(xTarget, yTarget)) {
					engine.gui->message(TCODColor::white, "You cannot see a way past!");
				} else {
					Actor *target = engine.map->getOccupant(xTarget, yTarget);
					engine.gui->message(TCODColor::white, "You cannot get past the %s.", target->name);
				}
				break;
		}

		return false;
	}
	// are we already charging some action?
	if (subject->tempo->getCurrState() == ActorClock::ClockState::CHARGING) {
		// if newAction matches currentAction, or if the player is using WAIT
		// to charge the action, charge it and finish
		if (subject->sentience->context->currAction == subject->tempo->getCurrAction() ||
				subject->sentience->context->currAction == Action::WAIT) {
			subject->tempo->chargeAction();
			return true;
		}
	}
	// we are not charging something, therefore we are performing a new action
	// that is, the ClockState is NO_ACTION || READY (!)
	// be sure that an action NEVER leaves this function while still READY!
	subject->tempo->changeAction(subject->sentience->context->currAction);
	subject->tempo->chargeAction(); // this should make an action READY if it is
	if (subject->tempo->getCurrState() == ActorClock::ClockState::CHARGING) {
		engine.gui->message(TCODColor::lightGrey, "You charge your action.");
		return true;
	}
	// else tempo->currState == READY
	// this line is here to assert against idiotic things later
//	assert(subject->tempo->getCurrState() != ActorClock::ClockState::NO_ACTION);
	// we were able to charge the action during this turn, so perform it
	switch(subject->sentience->context->currAction) {
		case Action::WAIT: // player will wait N = 1 turns
			// does nothing yet, not even pass time...
			subject->sentience->wait(subject);
			break;
		case Action::MOVE: // player will move to a new tile by relative coords
			if (context->isSuccessful()) {
				subject->sentience->moveRel(subject, context->echs, context->whye);
			}
			break;
		case Action::GROOM:
			if (context->target) {
				subject->sentience->groom(subject, context->target);
			} else {
				subject->sentience->groom(subject, subject);
			}
			break;
		// ***************
		case Action::IDLE: // always last!
			break;
		default:
			ERRMSG("Invalid action returned by parser");
			break;
	}
	// the action has been consumed, so reset the AP tracker
	if (subject->tempo->getCurrState() == ActorClock::ClockState::READY) {
		subject->tempo->resetAction();
	}
	// presumably now: subject->tempo->currState == NO_ACTION || CHARGING
	return true;
}
bool PlayerSentience::update(ActionContext context) {
	// this is probably a mapping resolution call
	// between the parser's reported action and the available sentience calls
	LOGMSG(" called by ActionContext context");
	return false;
}
/* PREVIOUS PLAYER FUNCTIONS
bool PlayerSentience::handleActionInput(Actor *subject, int inputKeystroke) {
	// handles all player keystroke event translation
	int xdiff, ydiff = 0;
	bool stateChange = false;
	switch(inputKeystroke) {
		case 'y': { // move NW
			xdiff = -1;
			ydiff = -1;
			break;
		}
		case 'u': { // move NE
			xdiff =  1;
			ydiff = -1;
			break;
		}
		case 'b': { // move SW
			xdiff = -1;
			ydiff =  1;
			break;
		}
		case 'n': { // move SE
			xdiff =  1;
			ydiff =  1;
			break;
		}
		case 'k': { // move player up
			ydiff = -1;
			break;
		}
		case 'j': { // move player down
			ydiff =  1;
			break;
		}
		case 'h': { // move player left
			xdiff = -1;
			break;
		}
		case 'l': { // move player right
			xdiff =  1;
			break;
		}
		case 'd': { //DROP item
			Actor *object = chooseFromInventory(subject);
			if (object) {
				object->portable->drop(object, subject);
//				engine.gameStatus = Engine::NEW_TURN;
			}
			stateChange = true;
			break;
		}
		case 'g': { //GET item
			bool found = false;
			for (Actor **iter = engine.actors.begin(); iter != engine.actors.end(); iter++) {
				Actor *object = *iter;
				// is the object get-able AND
				// AND is the subject standing over the object?
				if (object->portable && object->xpos == subject->xpos && actor->ypos == subject->ypos) {
					if (actor->portable->pickUp(subject, subject)) {
						found = true;
						engine.gui->message(TCODColor::lightGrey, "You pick up the %s.", object->name);
						break;
					} else if (!found) {
						found = true;
						engine.gui->message(TCODColor::red, "Your inventory is full.");
					}
				}
			}
			if (!found) {
				engine.gui->message(TCODColor::lightGrey, "There is nothing here to pick up.");
			}
			// Check the ground under the player's feet for things to GET
			// is there actually anything on this tile?
//			if (engine.map->tiles[subject->xpos + subject->ypos * engine.map->width]->itemList) { }
			// if there is, AND there's just the one thing, pick it up
			// if there's more than one object, invoke a menu
//			engine.gameStatus=Engine::NEW_TURN;
			stateChange = true;
			break;
		}
		case 'i': { //INVENTORY list
			// this method requires the player to be Portable, which makes no
			// sense at all - when would an NPC pick up the Player? - so the
			// logic for invoking the player's inventory self-container should
			// be moved elsewhere, probably into the Container class itself?
			Actor *actor = chooseFromInventory(subject);
			if (actor) {
				actor->portable->use(actor, subject);
//				engine.gameStatus = Engine::NEW_TURN;
			}
		break;
		}
		case 'p': { //DEBUG: Print player position
			LOGMSG("P-LOC abs: " << subject->xpos << ", " << subject->ypos);
			break;
		}
		case 'm': { //DEBUG: Spawn a monkey at a random screen location
			engine.map->addAnimal(subject->xpos - 5, subject->ypos - 5);
			break;
		}
	}
	// update if we've changed position
	if (xdiff != 0 || ydiff != 0) {
//		engine.gameStatus=Engine::NEW_TURN;
		// the if-then statement below prints spurious collision messages...
		engine.map->computeFOV();
		stateChange = true;
	}
	return stateChange;
}
bool PlayerSentience::decideMoveAttack(Actor *subject, int targetx, int targety) {
	// returns false if the player did not move
	// is there something in the way?
//	LOGMSG("TRY: Move player to (" << targetx << ", " << targety << ")");
	if (engine.map->isWall(targetx, targety)) {
		// there's a wall
		engine.gui->message(TCODColor::white, "You bump your nose.");
		return false;
	} else if (engine.map->isOccupied(targetx, targety)) {
		// there's something or someone
		engine.gui->message(TCODColor::white, "There's someone in the way.");
		return false;
	}
	// there's nothing in the way, so move the player
	subject->xpos = targetx;
	subject->ypos = targety;
//	LOGMSG("Player position: " << targetx << ", " << targety);
	LOGMSG("MOVE-TO: " << targetx << ", " << targety << " [" \
			<< (engine.map->isWall(targetx, targety) ? "wall" : "open") \
			<< ", " << (engine.map->isOccupied(targetx, targety) ? "actor" : "empty" ) \
			<< "]");
//	subject->tempo->deductAP(100);
	LOGMSG(subject->name << " deducted 100 AP");
	return true;
}
Actor *PlayerSentience::chooseFromInventory(Actor *subject) {
	// set up the inventory selection menu
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);
	// display the inventory menu frame
	con.setDefaultForeground(TCODColor::darkRed);
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "Inventory");
	// display the inventory list
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int index = 1;
	for (Actor **iter = subject->container->inventory.begin();
		iter != subject->container->inventory.end(); iter++) {
			Actor *object = *iter;
			con.printf(2, index, "(%c) %s", shortcut, object->name);
			shortcut++;
			index++;
	}
	// blit the console onto the main screen
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
		TCODConsole::root, (engine.screenWidth / 2) - (INVENTORY_WIDTH / 2),
		(engine.screenHeight / 2 ) - (INVENTORY_HEIGHT / 2));
	TCODConsole::flush();
	// wait for a keypress
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if (key.vk == TCODK_CHAR) {
		int actorIndex = key.c - 'a';
		if (actorIndex >= 0 && actorIndex < subject->container->inventory.size()) {
			return subject->container->inventory.get(actorIndex);
		}
	}
	return NULL;
}
*/
// **** ANIMAL Sentience
// **** utilities
AnimalSentience::AnimalSentience() {
	context = new ActionContext();
}
AnimalSentience::AnimalSentience(TCODZip &fileBuffer) {
	context = new ActionContext();
	context->load(fileBuffer);
}
bool AnimalSentience::update(Actor *subject) {
	// This is the core decision-making function: it will call any other
	// functions needed for an NPC to decide what to do on their turn
//	LOGMSG(subject->name << " updating");
	// if the subject is dead, don't even try (quietly)
	if (subject->mortality && subject->mortality->isDead()) {
		// might need to add logic that removes dead actors from engine queue
		return false;
	}
	// ***** NEW DECISION MAKING SYSTEM
	/* METHOD
	   1. Reassess goals
		A. Check emotional states to determine new goal priorities
		-- Internal states (satiety, etc) are queried at this stage
		-- External effects apply their emotional modifiers when they occur
		B. Check if the new goal is already achieved
		C. YES: push to bottom, goto B w/ new goal; NO: use that goal
	   2. (with UNACHIEVED Objective) consider what remains unachieved
	    A. Consult worldstate config to see what may be done
		B. If LOCAL worldstate does not support goal achievement...
		 i. Consult memory for DISTANT worldstate (UNIMPLEMENTED)
		ii. If nothing in memory, REASSESS GOALS
	   3. If goal is achievable at local scope, try to accomplish it
	    A. Uses a few metrics to assess success levels, modulated with heart[]
		B. Builds a stack of verb-contexts that it then tries to compare with
		C. At each turn, it compares the verb-context on top with its state
	   */

	// ***** OLD AD-HOC BEHAVIOR
	// this is some really rough ad-hoc stuff for testing simple AI features
	// all of it should be replaced with generalized behavior systems
	// DECIDE ON AN ACTION:
	// can the animal see the player?
/*	if (engine.map->isVisible(subject->xpos, subject->ypos)) {
		// the player is visible, time to get close and try to pet them
		// are we adjacent to the player?
		if (subject->getDistance(engine.player->xpos, engine.player->ypos) > 1) {
			// no, let's get closer
			// populate context with direction
			int targetx = engine.player->xpos - subject->xpos;
			int targety = engine.player->ypos - subject->ypos;
			float distance = sqrtf(targetx * targetx + targety * targety);
			if (distance >= 1) { // how far away is the target tile?
				// if it's more than a single step away, move in that direction
				// do not allow the actor to take more than 1 step in any direction
				targetx = (int)(round(targetx / distance));
				targety = (int)(round(targety / distance));
				if (targetx > 1) {
					targetx = targetx / targetx;
				} else if (targetx < -1) {
					targetx = targetx / -(targetx);
				}
				if (targety > 1) {
					targety = targety / targety;
				} else if (targety < -1) {
					targety = targety / -(targety);
				}
			}
			// store the calculated movement
			context->echs = targetx;
			context->whye = targety;
			subject->tempo->changeAction(Action::MOVE);
		} else {
			// yes, we're close, time to pet the kitty!!
			// populate context with target
			context->target = engine.player;
			subject->tempo->changeAction(Action::GROOM);
		}
	}*/
	// TRY TO PERFORM THE ACTION
	subject->tempo->chargeAction();
	if (subject->tempo->getCurrState() == ActorClock::ClockState::CHARGING) {
		// the actor is still charging up their action
//		LOGMSG(subject->name << " is charging their action :" << subject->tempo->getCurrAction());
	} else {
		// this line is here to assert against idiotic things later
//		assert(subject->tempo->getCurrState() == ActorClock::ClockState::NO_ACTION);
		// the actor's action is READY
		switch(subject->tempo->getCurrAction()) {
			case Action::WAIT:
				break;
			case Action::MOVE:
				subject->sentience->moveRel(subject, context->echs, context->whye);
//				LOGMSG(subject->name << " moved to " << subject->xpos << ", " << subject->ypos);
				break;
			case Action::GROOM:
				if (context->target) {
					subject->sentience->groom(subject, context->target);
				} else {
					subject->sentience->groom(subject, subject);
				}
				break;
			// ***************
			case Action::IDLE:
				// fall through
			default:
				break;
		}
	}
	if (subject->tempo->getCurrState() == ActorClock::ClockState::READY) {
		subject->tempo->resetAction();
	}
	// subject->tempo->currState == NO_ACTION || CHARGING
	return true;	// the action has been consumed, so reset the AP tracker
}
bool AnimalSentience::update(ActionContext context) {
	// this is probably a mapping resolution call
	// between the parser's reported action and the available sentience calls
	LOGMSG(" called by ActionContext context");
	return false;
}
// **** machine fxns
// **** emotions
Sentience::Emotion AnimalSentience::EmotionMachine::strongestFeeling() {
	// returns strongest emotion among the feelings[]
	// start with JOY and compare each to the next, keeping the larger
	Emotion biggestFeel = Sentience::Emotion::JOY;
	int howMuchFeel = intensity[(uint)Sentience::Emotion::JOY];
	for (uint index = 0; index != 8; index++) {
		if (howMuchFeel < intensity[index]) {
			biggestFeel = (Emotion)index;
			howMuchFeel = intensity[index];
		}
	}
	return biggestFeel;
}
void AnimalSentience::EmotionMachine::changeEmotion(Sentience::Emotion feeling, int delta) {
	// changes the intensity of a specified emotion by the delta
	int feelIndex = (int)feeling;
	intensity[feelIndex] += delta;
	if (intensity[feelIndex] < 0) { // did an emotion go into a negative score?
		delta = (-1) * intensity[feelIndex]; // get the abs val of the difference
		intensity[feelIndex] = 0; // cap the feeling at 0
		if (feelIndex <= 3) { // in the range of 0-3
			feelIndex += 4; // add 4 to get the opposing emotion
		} else { // >= 4
			feelIndex -= 4; // rmv 4 to get the opposing emotion
		}
		intensity[feelIndex] += delta; // raise the opposing emotion by the diff
	}
	// cap all emotion intensities at 255 for arbitrary reasons
	if (intensity[feelIndex] > 255) {
		intensity[feelIndex] = 255;
	}
}
void AnimalSentience::EmotionMachine::emotionDecay(int decayRate) {
	// iterates across the feelings, reducing each by decayRate (default -1)
	// the three highest feelings will not decay beyond 30/20/10 respectively
	// figure out which feelings are strongest right now
	uint firstFeel = (uint)Sentience::Emotion::JOY;
	uint secondFeel = (uint)Sentience::Emotion::TRUST;
	uint thirdFeel = (uint)Sentience::Emotion::FEAR;
	uint feelIndex = 0;
	for (feelIndex = 0; feelIndex < 8; feelIndex++) {
		if (intensity[feelIndex] >= intensity[firstFeel]) {
			firstFeel = feelIndex;
		} else if (intensity[feelIndex] >= intensity[secondFeel]) {
			secondFeel = feelIndex;
		} else if (intensity[feelIndex] >= intensity[thirdFeel]) {
			thirdFeel = feelIndex;
		}
	}
	// iterate again and perform the decay adjustment
	for (feelIndex = 0; feelIndex < 8; feelIndex++) {
		intensity[feelIndex] -= decayRate;
	}
	// perform adjustments towards minima thresholds
	if (intensity[firstFeel] < firstLevel) intensity[firstFeel] = firstLevel;
	if (intensity[secondFeel] < secondLevel) intensity[secondFeel] = secondLevel;
	if (intensity[thirdFeel] < thirdLevel) intensity[thirdFeel] = thirdLevel;
}
// **** goals
Sentience::Objective AnimalSentience::GoalMachine::firstPriority() {
	// returns the most important goal, after assessing its objective graph
	return (Sentience::Objective)0; // FIXME
}
void AnimalSentience::GoalMachine::changeWeight(Objective row, Objective col, int delta) {
	// changes a weight in the objective graph at row, col by delta
}
// **** decision-making tools
bool AnimalSentience::decideMoveAttack(Actor *subject, int targetx, int targety) {
	// FIX: the movement code does NOT prevent actors from occupying the same
	// tile as the player!
	// decides whether the NPC should move to or attack the target square
	int xdiff = targetx - subject->xpos;
	int ydiff = targety - subject->ypos;
	float distance = sqrtf(xdiff * xdiff + ydiff * ydiff);
	if (distance >= 1) { // how far away is the target tile?
		// if it's more than a single step away, move in that direction
		// do not allow the actor to take more than 1 step in any direction
		xdiff = (int)(round(xdiff / distance));
		ydiff = (int)(round(ydiff / distance));
		if (xdiff > 1) {
			xdiff = xdiff / xdiff;
		} else if (xdiff < -1) {
			xdiff = xdiff / -(xdiff);
		}
		if (ydiff > 1) {
			ydiff = ydiff / ydiff;
		} else if (ydiff < -1) {
			ydiff = ydiff / -(ydiff);
		}
	}
	targetx = subject->xpos + xdiff;
	targety = subject->ypos + ydiff;
/*	LOGMSG (subject->name \
			<< " @" << subject->xpos << "," << subject->ypos \
			<< " -> " << targetx << "(" << xdiff \
			<< "), " << targety << "(" << ydiff << ")");
	*/
	if (engine.map->isWall(targetx, targety)) {
		// there's a wall
		engine.gui->message(TCODColor::white, "The %s bumps into a wall.", subject->name);
		return true;
	} else if (engine.map->isOccupied(targetx, targety)) {
		// there's something or someone
		engine.gui->message(TCODColor::white, "The %s can't get past.", subject->name);
		return true;
	} else {
		subject->sentience->moveAbs(subject, targetx, targety);
	}
/*	LOGMSG(subject->name \
			<< "\n abs(" << subject->xpos << "," << subject->ypos << ")\n" \
			<< " tgt(" << targetx << "," << targety << ")\n" \
			<< "diff(" << xdiff << "," << ydiff << ")");
	*/
//	subject->xpos += xdiff;
//	subject->ypos += ydiff;
//	subject->tempo->deductAP(100);
//	LOGMSG(subject->name << " deducted 100 AP");
	return true;
}
// **** ActionContext -- general container for action context resolution
ActionContext::ActionContext():
	currAction(Sentience::Action::IDLE),
	prevAction(Sentience::Action::IDLE),
	successFlag(true),
	target(nullptr),
	echs(0), whye(0), zhee(0)
	{	}
void ActionContext::save(TCODZip &fileBuffer) {
	LOGMSG("called");
	fileBuffer.putInt((int)currAction); // casting to int lets us refer by index
	fileBuffer.putInt((int)prevAction);
	fileBuffer.putInt((int)successFlag);
	fileBuffer.putInt(target != nullptr);
//	if (target) fileBuffer.putInt(???); // i need a permanent identifier!
	fileBuffer.putInt(echs);
	fileBuffer.putInt(whye);
	fileBuffer.putInt(zhee);
}
void ActionContext::load(TCODZip &fileBuffer) {
	LOGMSG("called");
	currAction = ((Sentience::Action)fileBuffer.getInt());
	prevAction = ((Sentience::Action)fileBuffer.getInt());
	successFlag = ((bool)fileBuffer.getInt());
	bool hasTarget = fileBuffer.getInt();
//	if (hasTarget) target = fileBuffer.XXX;
	echs = fileBuffer.getInt();
	whye = fileBuffer.getInt();
	zhee = fileBuffer.getInt();
	LOGMSG("currAction: " << (int)currAction << std::endl\
			<< "prevAction: " << (int)prevAction << std::endl\
			<< "hasTarget: " << hasTarget << std::endl\
			<< "echs: " << echs << std::endl\
			<< "whye: " << whye << std::endl\
			<< "zhee: " << zhee);
}
void ActionContext::clear() {
	// currAction and prevAction are handled by setAction()
	successFlag = true;
	target = nullptr;
	echs = whye = zhee = 0;
}
void ActionContext::setAction(Sentience::Action newAction) {
	prevAction = currAction;
	currAction = newAction;
}
