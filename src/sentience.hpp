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
		/*enum class Action : uint {
			IDLE	= 0,	// parser is awaiting input
			WAIT	= 1,	// player wants to do nothing [for one turn]
			MOVE	= 2,	// basic movement action
			RUN		= 3,	// > player wants to move quickly
			SNEAK	= 4,	// > player wants to move quietly
			CONSUME	= 5,	// player is ingesting something
			LEAP	= 6,	// player is leaping to another tile
			POUNCE	= 7,	// player is leaping at another creature
			GRAPPLE	= 8,	// player wants to restrict a creature's movements
			ATTACK	= 9,	// player wants to deal damage to another creature
			BITE	= 10,	// > player is attacking with teeth
			SLASH	= 11,	// > player is attacking with front claws
			RAKE	= 12,	// > player is attacking with back claws
			LOOK	= 12,	// > player wants info on a distant tile
			GRAB	= 13,	// player wants to grab/manipulate something
			CARRY	= 14,	// > player is carrying a small obj in the jaws
			DRAG	= 15,	// > player is pulling a large object with the teeth
			DROP	= 16,	// player wants to leave an object on the ground
			RUB		= 17,	// player is leaving a scent mark on an object
			SIT		= 18,	// player wants to sit on the ground
			GROOM	= 19,	// player wants to groom [self | another creature]
			OPEN	= 20,	// player is opening an object
			CLOSE	= 21,	// player is closing an object
			FORCE	= 22,	// player is forcing a lock open
			CAST	= 23,	// player is going to cast a spell
			USE		= 24,	// player wants to invoke object's functionality
			DIP		= 25,	// player is immersing one object inside another
			PRAY	= 26,	// player is attempting to invoke the favor of the gods
			WEAR	= 27,	// player wants to put on an article of clothing
			WIELD	= 28,	// player wants to use a weapon for attack purposes
			INVENTORY= 29	// player wants to see what they're carrying
		};*/
		enum class Action : uint {
			IDLE =0,// parser is awaiting input
			WAIT,	// player wants to do nothing [for one turn]
			MOVE,	// basic movement action
			RUN	,	// > player wants to move quickly
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
		
		void save(TCODZip &fileBuffer); // save actor's context to file
		void load(TCODZip &fileBuffer); // load actor's context from file
		virtual bool update(Actor *subject) = 0;
		virtual bool update(ActionContext context) = 0;
//		virtual int getCheapestActionCost() = 0;
		// GENERAL ACTIONS
		void performAction(ActionContext context);
		void wait(Actor *subject, int numOfTurns = 1);
		void moveRel(Actor *subject, int targetx, int targety); // RELATIVE COORDS
		void moveAbs(Actor *subject, int targetx, int targety); // ABSOLUTE COORDS
		void drop(Actor *subject, Actor *target);
		void consume(Actor *subject, Actor *target); // change to only allow consumable actors?
		void grab(Actor *subject, int targetx, int targety); // ABSOLUTE COORDS
		void toggleSit(Actor *subject);
		void rubOn(Actor *subject, Actor *target); // includes all general scent deposit verbs
		void groom(Actor *subject, Actor *target); // social interaction, also allows self
		// NOT IMPLEMENTED:
		// open, close, force, cast, use, dip, pray, wear, wield, inventory

		ActionContext *context; // contains action details
	protected:
		enum SentienceType {
			PLAYER, PREY, PREDATOR, HUMAN
		};
};
class PlayerSentience: public Sentience {
	public:
		PlayerSentience();
		PlayerSentience(TCODZip &fileBuffer);
		bool update(Actor *subject);
		bool update(ActionContext context);
	protected:
		void farlook(int targetx, int targety); // ABSOLUTE COORDS
//		bool handleActionInput(Actor *subject, int inputKeystroke);
//		bool decideMoveAttack(Actor *subject, int targetx, int targety);
//		Actor *chooseFromInventory(Actor *subject);
};
class AnimalSentience: public Sentience {
	public:
		AnimalSentience();
		AnimalSentience(TCODZip &fileBuffer);
		bool update(Actor *subject);
		bool update(ActionContext context);
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
// NOTE: all actions are assumed to be successful unless specified otherwise!
struct ActionContext {
	// controls
	ActionContext();
	void save(TCODZip &fileBuffer);
	void load(TCODZip &fileBuffer);
	void clear();	// wipe the details in preparation for a new action
	void setAction(Sentience::Action newAction); // sets curr/prevAction
	void setSuccess(bool newValue) { successFlag = newValue; }
	bool isSuccessful() { return successFlag; }
	// contents
	Sentience::Action currAction;	// the action to be performed
	Sentience::Action prevAction;	// the previous action performed
	bool successFlag; // = false if an action is attempted and blocked
	Actor *target;	// the actor upon which the action will be performed
	int echs, whye, zhee;	// generic number containers
};
