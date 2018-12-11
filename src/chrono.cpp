/*	 chrono.hpp
DATE Dec 10 2018
AUTH xazak
DESC Definitions of the timekeeping systems, including world time and actor AP
 */
#include "main.hpp"

void ActorClock::changeAction(Sentience::Action newAction) {
	currAction = newAction;
	actionCost = APCostLookup[currAction];
	bankedAP = 0; // can change later to allow carryover
}
