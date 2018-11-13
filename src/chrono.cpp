/*FILE: chrono.cpp
  This file implements timekeeping functions across the entire game scope.
  */

#include "main.hpp"

GameClock::GameClock(uint sec, uint min, uint hrs, uint dys, uint mth, uint yrs)
	:seconds(sec), minutes(min), hours(hrs), days(dys), months(mth), years(yrs){
//		LOGMSG("Game clock init:" << years << ":" << months << ":" << days \
//		<< ":" << hours << ":" << minutes << ":" << seconds);
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
void GameClock::updateTurn() {
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
ActorClock::ActorClock(int newAPRate): currentAP(0), refreshRate(newAPRate) { }
int ActorClock::refreshAP() {
	currentAP += refreshRate;
	return currentAP;
}
int ActorClock::deductAP(int deduction) {
	currentAP -= deduction;
	return currentAP;
}
