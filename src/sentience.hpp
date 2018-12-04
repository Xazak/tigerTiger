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
struct ActionContext;
class Sentience {
	public:
		enum class Action {
			IDLE,	// parser is awaiting input
			WAIT,	// player wants to do nothing [for one turn]
			MOVE,	// basic movement action
			RUN,	// > player wants to move quickly
			SNEAK,	// > player wants to move quietly
			CONSUME,// player is ingesting something
			LEAP,	// player is leaping to another tile
			POUNCE,	// player is leaping at another creature
			GRAPPLE,// player wants to restrict a creature's movements
			ATTACK,	// player wants to deal damage to another creature
			BITE,	// > player is attacking with teeth
			SLASH,	// > player is attacking with front claws
			RAKE,	// > player is attacking with back claws
			LOOK,	// > player wants info on a distant tile
			GRAB,	// player wants to grab/manipulate something
			CARRY,	// > player is carrying a small obj in the jaws
			DRAG,	// > player is pulling a large object with the teeth
			DROP,	// player wants to leave an object on the ground
			RUB,	// player is leaving a scent mark on an object
			SIT,	// player wants to sit on the ground
			GROOM,	// player wants to groom [self | another creature]
			OPEN,	// player is opening an object
			CLOSE,	// player is closing an object
			FORCE,	// player is forcing a lock open
			CAST,	// player is going to cast a spell
			USE,	// player wants to invoke object's functionality
			DIP,	// player is immersing one object inside another
			PRAY,	// player is attempting to invoke the favor of the gods
			WEAR,	// player wants to put on an article of clothing
			WIELD,	// player wants to use a weapon for attack purposes
			INVENTORY // player wants to see what they're carrying
		};
		virtual bool update(Actor *subject) = 0;
		virtual bool update(ActionContext context) = 0;
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
		bool update(ActionContext context);
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
		bool decideMoveAttack(Actor *subject, int targetx, int targety);
};
class TigerAI: public Sentience {
	// PlayerSentience is a friend of this class (but not the other way!)
	friend class PlayerSentience;
	public:
		void leap(int targetx, int targety); // ABSOLUTE COORDS
		void slash(Actor *target);
};
struct ActionContext {
	// the actor performing the action is the actor who owns this object
	Sentience::Action action;	// the action to be performed
	Actor *target;	// the actor upon which the action will be performed
	int echs, whye, zhee;	// generic number containers
	void clear();	// wipe the details in preparation for a new action
};
