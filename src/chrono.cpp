/*	 chrono.cpp
DATE Dec 10 2018
AUTH xazak
DESC Implementations of the timekeeping systems, including world time and actor AP
 */
#include "main.hpp"

// **** WorldClock
WorldClock::WorldClock():
	years(1823),
	months(4),
	days(13),
	hours(7),
	minutes(22),
	seconds(36),
	turns(0) { }
void WorldClock::advanceTime(uint increment) {
	turns++;
	seconds += increment;
	if (seconds >= 60) {
		minutes++;
		seconds %= 60;
	}
	if (minutes >= 60) {
		hours++;
		minutes %= 60;
	}
	if (hours >= 24) {
		days++;
		hours %= 24;
	}
	if (days >= 30) {
		months++;
		days %= 30;
	}
	if (months >= 12) {
		years++;
		months %= 12;
	}
}
void WorldClock::save(TCODZip &fileBuffer) {
	LOGMSG("called");
	fileBuffer.putInt(years);
	fileBuffer.putInt(months);
	fileBuffer.putInt(days);
	fileBuffer.putInt(hours);
	fileBuffer.putInt(minutes);
	fileBuffer.putInt(seconds);
	fileBuffer.putInt(turns);
}
void WorldClock::load(TCODZip &fileBuffer) {
	LOGMSG("called");
	setYears((uint)fileBuffer.getInt());
	setMonths((uint)fileBuffer.getInt());
	setDays((uint)fileBuffer.getInt());
	setHours((uint)fileBuffer.getInt());
	setMinutes((uint)fileBuffer.getInt());
	setSeconds((uint)fileBuffer.getInt());
	setTurns((uint)fileBuffer.getInt());
}
// **** ActorClock
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
