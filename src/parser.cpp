/* parser.cpp
DATE Nov 30, 2018
AUTH xazak
DESC Implementation of the parser module.
 */

#include "main.hpp"

CmdInterpreter::CmdInterpreter() {
}
//~CmdInterpreter::CmdInterpreter() { }
void CmdInterpreter::changeAction(Action newAction) {
	prevAction = currAction;
	currAction = newAction;
}
void CmdInterpreter::translate() {
	// reads input data from lastEvent/lastKey and sets the parser's state
	changeAction(keycodeLookup[lastKey.c]);
	stateChange = actionDiegesis[currAction];
}
