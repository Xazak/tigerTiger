/*   parser.hpp
DATE Nov 30, 2018
AUTH xazak
DESC Definitions for the parser module, which translates keystrokes into game actions.
 */
#include <map>
class CmdInterpreter {
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
		std::map<char, Action> keycodeLookup = {
//			{ 'a', Action::FOO},
			{ 'b', Action::MOVE},
			{ 'c', Action::CONSUME},
			{ 'd', Action::DROP},
//			{ 'e', Action::FOO},
//			{ 'f', Action::FOO},
			{ 'g', Action::GRAB},
			{ 'h', Action::MOVE},
			{ 'i', Action::INVENTORY},
			{ 'j', Action::MOVE},
			{ 'k', Action::MOVE},
			{ 'l', Action::MOVE},
//			{ 'm', Action::FOO},
			{ 'n', Action::MOVE},
//			{ 'o', Action::FOO},
//			{ 'p', Action::FOO},
//			{ 'q', Action::FOO},
			{ 'r', Action::RUB},
			{ 's', Action::SIT},
//			{ 't', Action::FOO},
			{ 'u', Action::MOVE},
//			{ 'v', Action::FOO},
//			{ 'x', Action::FOO},
//			{ 'w', Action::FOO},
			{ 'y', Action::MOVE},
			{ 'z', Action::WAIT},
//			{ 'A', Action::FOO},
			{ 'G', Action::GROOM},
			{ 'L', Action::LEAP},
			{ ';', Action::LOOK}
		};
		std::map<Action, bool> actionDiegesis = {
			{ Action::IDLE, false},
			{ Action::WAIT, true},
			{ Action::MOVE, true},
			{ Action::RUN, true},
			{ Action::SNEAK, true},
			{ Action::CONSUME, true},
			{ Action::LEAP, true},
			{ Action::POUNCE, true},
			{ Action::GRAPPLE, true},
			{ Action::ATTACK, true},
			{ Action::BITE, true},
			{ Action::SLASH, true},
			{ Action::RAKE, true},
			{ Action::LOOK, false}, // instant
			{ Action::GRAB, false},
			{ Action::CARRY, true}, // > pick something up
			{ Action::DRAG, true},  // > drag something along
			{ Action::DROP, true}, // instant
			{ Action::RUB, true},
			{ Action::SIT, true},
			{ Action::GROOM, true},
			{ Action::OPEN, true},
			{ Action::CLOSE, true},
			{ Action::FORCE, true},
			{ Action::CAST, true},
			{ Action::USE, true},
			{ Action::DIP, true},
			{ Action::PRAY, true},
			{ Action::WEAR, true},
			{ Action::WIELD, true},
			{ Action::INVENTORY, false}
		};
		TCOD_event_t lastEvent; // contains last input event from player
		TCOD_key_t lastKey; // contains last key pressed by player
		// lastKey.vk = TCOD keycode (as enum), lastKey.c = printable char
		TCOD_mouse_t mouse; // contains mouse input
		bool stateChange = false;

		CmdInterpreter();
//		~CmdInterpreter();
		void translate();
		void changeAction(Action newAction);
		Action getCurrAction() { return currAction; };
		Action getPrevAction() { return prevAction; };

	private:
		Action currAction;
		Action prevAction;

};
extern CmdInterpreter parser;
