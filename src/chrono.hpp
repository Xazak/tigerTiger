/*FILE: chrono.hpp
  This file describes the timekeeping functions across the entire game scope,
  from the low-level milliturn level up through high-level calendar tools.
  */
class GameClock {
	public:
		TCODList<Actor *> actionQueue; // contains NEARBY, SENTIENT actors
		GameClock(uint sec = 0, uint min = 0, uint hrs = 0, uint dys = 0,
				uint mth = 0, uint yrs = 0);
//		~GameClock();
		// how should we return the game date? const string pointer?
		// allows adjustment of date; calling w/ no args moves time by +1 sec
		int refreshActionQueue();
		void updateCalendar(int increment = 1); // updates the game calendar
		void updateTurn(); // invokes the update routines for all actors

	protected:
		// The turn count and the game date are intentionally segregated from
		// each other! this allows things like starting from turn 0 but the
		// year 1041, for example.
		// Use updateCalendar() for the game date and newTurn() for game turns.
		// this is 32 bits: if 1 turn / 1 sec, will max out at ~136 years
		unsigned long int turnCount = 0;
		uint seconds;
		uint minutes;
		uint hours;
		uint days;
		uint months;
		uint years;
};
class ActorClock {
	public:
		ActorClock(int newAPRate);
//		~ActorClock();
		int refreshAP();
		int deductAP(int deduction);
		bool hasEnergy() { return (currentAP >= minimumAPCost); }
		// sets/gets
		void setCurrentAP(uint newValue) { currentAP = newValue; }
		int getCurrentAP() { return currentAP; }
		void setRefreshRate(uint newValue) { refreshRate = newValue; }
		int getRefreshRate() { return refreshRate; }
		void setMinimumAPCost(int newValue) { minimumAPCost = newValue; }
		int getMinimumAPCost() { return minimumAPCost; }
	protected:
		int currentAP;
		int refreshRate;
		int minimumAPCost = 1; // = the cost of the cheapest action available to the actor
};
