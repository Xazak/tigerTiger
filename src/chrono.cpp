/*	 chrono.cpp
DATE Dec 10 2018
AUTH xazak
DESC Implementations of the timekeeping systems, including world time and actor AP
 */
#include "main.hpp"

ActorClock::ActorClock(int newRefreshRate):
	currAction(Sentience::Action::IDLE),
	currState(ClockState::NO_ACTION),
	actionCost(0),
	bankedAP(0),
	refreshRate(newRefreshRate),
	currentAP(0)
	{ }
ActorClock::ActorClock(TCODZip &fileBuffer) {
	LOGMSG("called");
	load(fileBuffer);
}
void ActorClock::save(TCODZip &fileBuffer) {
	LOGMSG("called");
	fileBuffer.putInt((uint)currState);
	fileBuffer.putInt((uint)currAction);
	fileBuffer.putInt(actionCost);
	fileBuffer.putInt(bankedAP);
	fileBuffer.putInt(refreshRate);
	fileBuffer.putInt(currentAP);
}
void ActorClock::load(TCODZip &fileBuffer) {
	LOGMSG("called");
	// currState should be either ClockState::CHARGING or ClockState::READY if invoking this!
	// currState should have been set before invoking this!
	changeState((ActorClock::ClockState)fileBuffer.getInt());
	changeAction((Sentience::Action)fileBuffer.getInt());
	actionCost = fileBuffer.getInt();
	bankedAP = fileBuffer.getInt();
	refreshRate = fileBuffer.getInt();
	currentAP = fileBuffer.getInt();
	LOGMSG("currState: " << (int)currState << std::endl\
			<< "currAction: " << (int)currAction << std::endl\
			<< "actionCost: " << actionCost << std::endl\
			<< "bankedAP: " << bankedAP << std::endl\
			<< "refreshRate: " << refreshRate << std::endl\
			<< "currentAP: " << currentAP);
}
// the sentience module will consume the readied action and change the state
/*
AP METHOD
1	Actor decides to take an action
2	Actor asks: Do I have enough AP? (currState?)
3	IF actor.currAP < actionCost
3.2		bankedAP += actor.currAP;
3.4		actor.currAP = 0;
4	ELSE [[ actor.currAP >= actionCost ]]
4.2		actor.currAP -= actionCost;
*/
void ActorClock::changeState(ClockState newState) {
	currState = newState;
//	LOGMSG("tempo state: " << currState);
}
void ActorClock::changeAction(Sentience::Action newAction) {
	currAction = newAction;
	actionCost = APCostLookup[currAction];
	changeState(ClockState::CHARGING);
}
ActorClock::ClockState ActorClock::chargeAction() {
	// does the action cost more than the player has?
	// if so, bank the AP towards the future action
	if (currentAP < actionCost) {
		bankedAP += currentAP;
		currentAP = 0;
	} else { // the player can pay for the action immediately (currAP >= cost)
		currentAP -= actionCost;
		changeState(ClockState::READY);
	}
	if (bankedAP > actionCost) {
		changeState(ClockState::READY);
	}
	return currState;
}
void ActorClock::resetAction() {
	currAction = Sentience::Action::IDLE;
	changeState(ClockState::NO_ACTION);
	actionCost = 0;
	bankedAP = 0;
}
