#include <stdio.h> //?
#include "main.hpp"

Mortality::Mortality(float newMaxHP, float newDefense, const char *cadaverTitle):
	maximumHP(newMaxHP), currentHP(newMaxHP), defense(newDefense) {
		this->cadaverTitle = strdup(cadaverTitle);
}
Mortality::~Mortality() {
	free((char *)cadaverTitle);
}
void Mortality::embraceDeath(Actor *subject) {
	// change the actor's sigil into a corpse
	subject->sigil = '%';
	subject->color = TCODColor::darkRed;
	subject->name = cadaverTitle;
	subject->obstructs = false;
	// ensure that corpses are rendered before living actors (ie, underneath)
	engine.sendToBack(subject);
}
float Mortality::adjustHP(Actor *subject, float hpAdjustment) {
	// This function performs all HP adjustments, both damage and healing
	// Messages regarding these adjustments are left up to the abilities that
	// caused the adjustment in the first place
	// Prevent overhealing (ie only heal up to maxHP)
	if ((currentHP + hpAdjustment) > maximumHP) { hpAdjustment = maximumHP - currentHP; }
	currentHP += hpAdjustment; // perform the adjustment
	return hpAdjustment;
}
// SUBTYPES
// Player
PlayerMortality::PlayerMortality (float newMaxHP, float newDefense,
	const char *cadaverTitle):
	Mortality(newMaxHP, newDefense, cadaverTitle) { }
void PlayerMortality::embraceDeath(Actor *subject) {
	engine.gui->message(TCODColor::red, "You died!\n");
	Mortality::embraceDeath(subject);
	engine.gameStatus = Engine::DEFEAT;
}
// NPC
NPCMortality::NPCMortality (float newMaxHP, float newDefense, const char *cadaverTitle):
	Mortality(newMaxHP, newDefense, cadaverTitle) { }
void NPCMortality::embraceDeath(Actor *subject) {
	engine.gui->message(TCODColor::lightGrey, "%s is dead.\n", subject->name);
	Mortality::embraceDeath(subject);
}
