/*	 sentience.hpp
DATE Oct 24 2018
AUTH xazak
DESC Contains action packages that provide decision-making tools for Actors.
 */
/* STRUCTURE AND NOTES
   In general, the update() function is the primary decision maker; it may
   invoke other modules to help it make a better choice
   The update() function has a list of the actions available to it, and uses it
   to decide which action to take
   When update() knows what it wants to do, it invokes the proper action
   PERFORM ALL ACTION CHECKING EITHER IN THE PARSER (player) OR UPDATE() (npc)
   */
class Sentience {
	public:
		virtual bool update(Actor *subject) = 0;
		virtual int getCheapestActionCost() = 0;
		// GENERAL ACTIONS
		void wait(int numOfTurns = 1);
		void move(int targetx, int targety); // ABSOLUTE COORDS
		void drop(Actor *target);
		void consume(Actor *target); // change to only allow consumable actors?
		void grab(int targetx, int targety); // ABSOLUTE COORDS
		void toggleSit();
		void rubOn(Actor *target); // includes all general scent deposit verbs
		void groom(Actor *target); // social interaction, also allows self
		// NOT IMPLEMENTED:
		// open, close, force, cast, use, dip, pray, wear, wield, inventory

	protected:
		enum SentienceType {
			PLAYER, PREY, PREDATOR, HUMAN
		};
};
class PlayerSentience: public Sentience {
	public:
		bool update(Actor *subject);
		int getCheapestActionCost();
	protected:
		void farlook(int targetx, int targety); // ABSOLUTE COORDS
//		bool handleActionInput(Actor *subject, int inputKeystroke);
//		bool decideMoveAttack(Actor *subject, int targetx, int targety);
//		Actor *chooseFromInventory(Actor *subject);
};
class AnimalSentience: public Sentience {
	public:
		bool update(Actor *subject);
		int getCheapestActionCost();
	protected:
//		bool decideMoveAttack(Actor *subject, int targetx, int targety);
};
class TigerAI: public Sentience {
	// PlayerSentience is a friend of this class (but not the other way!)
	friend class PlayerSentience;
	public:
		void leap(int targetx, int targety); // ABSOLUTE COORDS
		void slash(Actor *target);
}
