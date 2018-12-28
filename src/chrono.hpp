/*	 chrono.hpp
DATE Dec 10 2018
AUTH xazak
DESC Definitions of the timekeeping systems, including world time and actor AP
 */
#include <map>
class WorldClock {
	// handles the game clock functions, including calendar and time tools

};
class ActorClock {
	// handles the AP system for any Actor it is attached to
	// METHOD
	// -- All actions cost a certain amount of AP
	// -- All actors are granted a certain amount of AP each turn
	// -- If an actor has any leftover AP at the end of a turn, it is discarded
	// -- If an actor does not have enough AP to execute an action, it may be
	// banked forward towards that action's execution in the future
	// -- All actions can be banked: this allows slower creatures to work!
	public:
		std::map<Sentience::Action, int> APCostLookup = {
			{Sentience::Action::IDLE, 0},
			{Sentience::Action::WAIT, 10},
			{Sentience::Action::MOVE, 100},
			{Sentience::Action::RUN, 25},
			{Sentience::Action::SNEAK, 130},
			{Sentience::Action::CONSUME, 100},
			{Sentience::Action::LEAP, 100},
			{Sentience::Action::POUNCE, 100},
			{Sentience::Action::GRAPPLE, 100},
			{Sentience::Action::ATTACK, 100},
			{Sentience::Action::BITE, 100},
			{Sentience::Action::SLASH, 100},
			{Sentience::Action::RAKE, 100},
			{Sentience::Action::GRAB, 100},
			{Sentience::Action::CARRY, 0}, // > pick something up
			{Sentience::Action::DRAG, 100},  // > drag something along
			{Sentience::Action::DROP, 0}, // instant?
			{Sentience::Action::RUB, 100},
			{Sentience::Action::SIT, 50},
			{Sentience::Action::GROOM, 100},
			{Sentience::Action::OPEN, 100},
			{Sentience::Action::CLOSE, 100},
			{Sentience::Action::FORCE, 100},
			{Sentience::Action::CAST, 100},
			{Sentience::Action::USE, 100},
			{Sentience::Action::DIP, 100},
			{Sentience::Action::PRAY, 100},
			{Sentience::Action::WEAR, 100},
			{Sentience::Action::WIELD, 100},
		};
		enum class ClockState : uint {
			NO_ACTION,
			CHARGING,
			READY
		};

		ActorClock(int newRefreshRate);
		ActorClock(TCODZip &fileBuffer);
		void save(TCODZip &fileBuffer);
		void load(TCODZip &fileBuffer);

		void refreshAP() { currentAP = refreshRate; }
		void changeState(ClockState newState);
		void changeAction(Sentience::Action newAction);
		ClockState chargeAction();
		void resetAction();

		ClockState getCurrState() { return currState; }
		Sentience::Action getCurrAction() { return currAction; }
		int getActionCost() { return actionCost; }
		int getBankedAP() { return bankedAP; }
		int getRefreshRate() { return refreshRate; }
		int getCurrAP() { return currentAP; }

		void setActionCost(int newValue) { actionCost = newValue; }
		void setRefreshRate(int newValue) { refreshRate = newValue; }
		void setCurrAP(int newValue) { currentAP = newValue; }

	private:
		Sentience::Action currAction; // the action being banked towards
		ClockState currState; // to check if charging an action, etc
		int actionCost; // the total AP needed to perform the current action
		int bankedAP; // the AP currently saved up towards the current action
		int refreshRate; // the amt of AP granted by the engine every turn
		int currentAP; // the amt of remaining AP available to the actor
};
