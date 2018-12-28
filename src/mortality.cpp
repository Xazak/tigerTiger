/*	 mortality.hpp
DATE Oct 25 2018
AUTH xazak
DESC Definitions of functions related to hit point adjustment and death.
 */
#include "main.hpp"
#include <stdio.h> //?

Mortality::Mortality(float newMaxHP, float newDefense, const char *newName):
	maximumHP(newMaxHP),
	currentHP(newMaxHP),
	defense(newDefense) {
	if (newName == nullptr) {
		this->cadaverTitle = "nobody's corpse";
	} else {
		this->cadaverTitle = strdup(newName);
	}
}
Mortality::Mortality(TCODZip &fileBuffer) {
	LOGMSG("called");
	this->load(fileBuffer);
}
Mortality::~Mortality() {
	free((char *)cadaverTitle);
}
void Mortality::save(TCODZip &fileBuffer) {
	LOGMSG("called");
	fileBuffer.putString(cadaverTitle);
	fileBuffer.putInt(maximumHP);
	fileBuffer.putInt(currentHP);
	fileBuffer.putInt(defense);
}
void Mortality::load(TCODZip &fileBuffer) {
	LOGMSG("called");
	cadaverTitle = strdup(fileBuffer.getString());
	maximumHP = fileBuffer.getInt();
	currentHP = fileBuffer.getInt();
	defense = fileBuffer.getInt();
	LOGMSG("corpse name: " << cadaverTitle << std::endl\
			<< "max HP: " << maximumHP << std::endl\
			<< "current HP: " << currentHP << std::endl\
			<< "defense: " << defense);
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
	// Changes the subject's HP by a relative amount: positive or negative
	// Messages regarding these adjustments are left up to the abilities that
	// caused the adjustment in the first place
	// Prevent overhealing (ie only heal up to maxHP)
	if (hpAdjustment == 0) return hpAdjustment; // take care of the zero case
	if ((currentHP + hpAdjustment) > maximumHP) { hpAdjustment = maximumHP - currentHP; }
	currentHP += hpAdjustment; // perform the adjustment
	return hpAdjustment; // report the adjusted HP amount in case we need it
}
// SUBTYPES
// Player
PlayerMortality::PlayerMortality (float newMaxHP, float newDefense,
	const char *cadaverTitle):
	Mortality(newMaxHP, newDefense, cadaverTitle) { }
void PlayerMortality::embraceDeath(Actor *subject) {
	engine.gui->message(TCODColor::red, "You died!\n");
	Mortality::embraceDeath(subject);
}
// NPC
NPCMortality::NPCMortality (float newMaxHP, float newDefense,
	const char *cadaverTitle):
	Mortality(newMaxHP, newDefense, cadaverTitle) { }
void NPCMortality::embraceDeath(Actor *subject) {
	engine.gui->message(TCODColor::lightGrey, "%s is dead.\n", subject->name);
	Mortality::embraceDeath(subject);
}
