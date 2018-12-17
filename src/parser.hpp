/*   parser.hpp
DATE Nov 30, 2018
AUTH xazak
DESC Definitions for the parser module, which translates keystrokes into game actions.
 */
#include <map>
class CmdInterpreter {
	public:
		std::map<char, Sentience::Action> keycodeLookup = {
//			{ 'a', Sentience::Action::FOO},
			{ 'b', Sentience::Action::MOVE},
			{ 'c', Sentience::Action::CONSUME},
			{ 'd', Sentience::Action::DROP},
//			{ 'e', Sentience::Action::FOO},
//			{ 'f', Sentience::Action::FOO},
			{ 'g', Sentience::Action::GRAB},
			{ 'h', Sentience::Action::MOVE},
			{ 'i', Sentience::Action::INVENTORY},
			{ 'j', Sentience::Action::MOVE},
			{ 'k', Sentience::Action::MOVE},
			{ 'l', Sentience::Action::MOVE},
//			{ 'm', Sentience::Action::FOO},
			{ 'n', Sentience::Action::MOVE},
//			{ 'o', Sentience::Action::FOO},
//			{ 'p', Sentience::Action::FOO},
//			{ 'q', Sentience::Action::FOO},
			{ 'r', Sentience::Action::RUB},
			{ 's', Sentience::Action::SIT},
//			{ 't', Sentience::Action::FOO},
			{ 'u', Sentience::Action::MOVE},
//			{ 'v', Sentience::Action::FOO},
//			{ 'x', Sentience::Action::FOO},
//			{ 'w', Sentience::Action::FOO},
			{ 'y', Sentience::Action::MOVE},
			{ 'z', Sentience::Action::WAIT},
//			{ 'A', Sentience::Action::FOO},
			{ 'G', Sentience::Action::GROOM},
			{ 'L', Sentience::Action::LEAP},
			{ ';', Sentience::Action::LOOK}
		};
		std::map<Sentience::Action, bool> actionDiegesis = {
			{Sentience::Action::IDLE, false},
			{Sentience::Action::WAIT, true},
			{Sentience::Action::MOVE, true},
			{Sentience::Action::RUN, true},
			{Sentience::Action::SNEAK, true},
			{Sentience::Action::CONSUME, true},
			{Sentience::Action::LEAP, true},
			{Sentience::Action::POUNCE, true},
			{Sentience::Action::GRAPPLE, true},
			{Sentience::Action::ATTACK, true},
			{Sentience::Action::BITE, true},
			{Sentience::Action::SLASH, true},
			{Sentience::Action::RAKE, true},
			{Sentience::Action::LOOK, false}, // instant
			{Sentience::Action::GRAB, false},
			{Sentience::Action::CARRY, true}, // > pick something up
			{Sentience::Action::DRAG, true},  // > drag something along
			{Sentience::Action::DROP, true}, // instant
			{Sentience::Action::RUB, true},
			{Sentience::Action::SIT, true},
			{Sentience::Action::GROOM, true},
			{Sentience::Action::OPEN, true},
			{Sentience::Action::CLOSE, true},
			{Sentience::Action::FORCE, true},
			{Sentience::Action::CAST, true},
			{Sentience::Action::USE, true},
			{Sentience::Action::DIP, true},
			{Sentience::Action::PRAY, true},
			{Sentience::Action::WEAR, true},
			{Sentience::Action::WIELD, true},
			{Sentience::Action::INVENTORY, false}
		};
//		CmdInterpreter();
//		~CmdInterpreter();
		void translate();
		void changeAction(Sentience::Action newAction);
		Sentience::Action getCurrAction() { return currAction; };
		Sentience::Action getPrevAction() { return prevAction; };

		TCOD_event_t lastEvent; // contains last input event from player
		TCOD_key_t lastKey; // contains last key pressed by player
		// lastKey.vk = TCOD keycode (as enum), lastKey.c = printable char
		TCOD_mouse_t mouse; // contains mouse input
		bool stateChange = false; // indicates meta context of player's input
		ActionContext context; // contains action details

	private:
		Sentience::Action currAction;
		Sentience::Action prevAction;

};
extern CmdInterpreter parser;
