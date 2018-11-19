/*FILE: chrono.cpp
  This file implements timekeeping functions across the entire game scope.
  */

#include "main.hpp"

GameClock::GameClock(uint sec, uint min, uint hrs, uint dys, uint mth, uint yrs)
	:seconds(sec), minutes(min), hours(hrs), days(dys), months(mth), years(yrs){
/*		LOGMSG("Game clock init:" << years << ":" << months << ":" << days \
		<< ":" << hours << ":" << minutes << ":" << seconds);
	*/
	}
void GameClock::updateCalendar(int increment) {
	// call this function with the number of seconds to increase the clock by
	// if no argument is supplied, will advance the game clock by 1 second
	// it should be possible to use any arbitrary timekeeping system by
	// changing the conversion ratios below or adding/removing them entirely
	seconds += increment;
	if (seconds >= 60) {
		seconds = 0;
		minutes++;
	}
	if (minutes >= 60) {
		minutes = 0;
		hours++;
	}
	if (hours >= 24) {
		hours = 0;
		days++;
	}
	if (days >= 30) {
		days = 0;
		months++;
	}
	if (months >= 12) {
		months = 0;
		years++;
	}
}
int GameClock::refreshActionQueue(bool refreshPools) {
	// returns the number of actors who will be taking actions (=size of queue)
	int numberOfActors = 0;
	// right now, all this does is copy the list of sentient actors from the engine
	// later, we will narrow the scope such that it only gets local actors
	// if we started a new turn, flush the queue and rebuild it
//	if (engine.gameStatus == Engine::NEW_TURN || engine.gameStatus == Engine::STARTUP) {
	if (refreshPools == true) {
		actionQueue.clear();
		// fill the queue
		for (Actor **iter = engine.actors.begin(); iter != engine.actors.end(); iter++) {
			Actor *actor = *iter;
			if (engine.gameStatus == Engine::NEW_TURN) actor->tempo->refreshAP();
			actionQueue.push(actor);
			numberOfActors++;
		}
	}
	// sort the queue if there's anyone besides the player
	if (actionQueue.size() > 1) {
		Actor **iter = actionQueue.begin();
		Actor *subject = NULL;
		Actor *neighbor = NULL;
		int actorsMoved = 0;
		do { // perform sorting operations until nothing is moved around
			if (iter == actionQueue.end()) {
				// reset the iterator and start sorting again
				iter = actionQueue.begin();
				actorsMoved = 0;
			}
			subject = *iter;
			neighbor = *iter + 1;
			if (subject->tempo->getCurrentAP() < neighbor->tempo->getCurrentAP()) {
				// swap their positions
				actorsMoved++;
			}
			iter++;
		} while (actorsMoved != 0);
	}
	return numberOfActors;
}
/*void GameClock::updateTurn() {
	// This function polls all local actors for their per-turn actions,
	// starting with the player
	// Each actor's update fxn does the math for how many actions they get
	engine.player->update();
	// then the NPCs make their moves
	if (engine.gameStatus == Engine::NEW_TURN) {
		for (Actor **iter = engine.actors.begin(); iter != engine.actors.end();
			iter++) {
			Actor *actor = *iter;
			if (actor != engine.player) {
				actor->update();
			}
		}
	}
}
void GameClock::updateTurn() {
	// This function handles the actor action queue
	// wait for the player to do something worth updating the game state about
//	engine.player->tempo->refreshAP();
//	engine.player->update();
	// refresh the local action queue
	actionQueue.clear(); // could this be conditional, to prevent extra updates?
	for (Actor **iter = engine.actors.begin(); iter != engine.actors.end(); iter++) {
		Actor *actor = *iter;
		// right now, all this cares about is whether the actors have tempo
		// later, when the map is large enough to need it, we can refine this
		// to differentiate between local/distant and in/animate  actors
		if (actor != engine.player && actor->tempo) actionQueue.push(actor);
	}
	// grant AP to all actors in the queue
	for (Actor **iter = actionQueue.begin(); iter != actionQueue.end(); iter++) {
		Actor *actor = *iter;
		actor->tempo->refreshAP();
	}
	// organize the queue by: AP remaining, proximity to player
	// process the action queue
	for (Actor **iter = actionQueue.begin(); iter != actionQueue.end(); iter++) {
		Actor *actor = *iter;
		//ask the actor at front of queue to update
		actor->update();
		//move the front-most actor back in the queue by their remaining AP
	}
	//declare the turn to be over, allow other game fxns to process

	//NOTES:
	// when is the game clock advanced? how often?
	// actors need intermediate states for charging actions, being paralyzed...
}*/
ActorClock::ActorClock(int newAPRate): currentAP(0), refreshRate(newAPRate) {
	// this space left blank
}
int ActorClock::refreshAP() {
	currentAP += refreshRate;
	return currentAP;
}
int ActorClock::deductAP(int deduction) {
	currentAP -= deduction;
	return currentAP;
}
