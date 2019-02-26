/*	 vitality.cpp
DATE Nov 09 2018
AUTH xazak
DESC This file implements the baseline statistics carried by every creature.
 */

#include "main.hpp"
#include <math.h>

// **** CONSUMABLE
Consumable::Consumable(uint inputCalories):
	caloricValue(inputCalories)
	{

	}
uint Consumable::ingest() {
	// returns the caloric value and removes the consumable/deducts a portion
	// effects that occur due to ingesting the item should activate here
	return caloricValue;
}

void Consumable::save(TCODZip &fileBuffer) {
	// save to file
	LOGMSG("called");
	fileBuffer.putInt(caloricValue);
}
void Consumable::load(TCODZip &fileBuffer) {
	// load from file
	LOGMSG("called");
	caloricValue = fileBuffer.getInt();
}

// **** VITALITY
Vitality::Vitality() :
	ferocity(5), tenacity(5), swiftness(5),
	quiescence(5), perception(5), tailLength(30),
	currSatiety(100), maxSatiety(100),
	currThirst(100), maxThirst(100),
	currSleep(100), maxSleep(100) { }
void Vitality::save(TCODZip &fileBuffer) {
	LOGMSG("called");
}
void Vitality::load(TCODZip &fileBuffer) {
	LOGMSG("called");
}
uint Vitality::getLeapDistance() {
	// returns a leap distance in tiles
	uint finalDistance = 0;
	// the secret sauce: combo of str + speed -> dist in m -> dist in tiles
	finalDistance = ((5 * log((this->tenacity + this->swiftness) / 2) + 5) / 4);
	return finalDistance;
}
uint Vitality::getSprintSpeed() {
	// returns a sprint speed in tiles/second
	uint finalSpeed = 0;
	// the secret sauce: combo of str + speed to get a val in kmh -> tiles/sec
	finalSpeed = ((log((this->tenacity + this->swiftness) / 2 ) * 20) / 4);
	return finalSpeed;
}
void Vitality::metabolize() {
	// Performs bookkeeping for calorie use and water intake
	// This routine should be called AFTER an action is performed!
	// Otherwise, inaccurate data will be used in the calculations.
	// Note that functions related to muscle/fat increase are located with the
	// food consumption routines! This function expends the food and water
	// resources to pay for the next turn's AP and replenish internal fluids

	// The baseline average, such that an adult tiger needs ~6 kg meat / day
	// (@4Kcal/g, ~24000 Kcal/day) is given by C(x) = 16x where x = seconds
	LOGMSG("called");

	// METHOD
	// collect information about what action the actor just took
	// determine the caloric cost of the action
	// deduct the cost from the actor's calorie pool
	// adjust the actor's fatigue/sleep levels
	// check new hunger/sleep level and change AP regen rate if needed
	// deduct baseline thirst from water level
	// deduct additional thirst to replenish blood vol up to maximum
}
//void Vitality::burnCalories();
void Vitality::gainEnergy(uint calories) {
	// adds the ingested calories to the actor's satiety meter
	currSatiety += calories;
}
