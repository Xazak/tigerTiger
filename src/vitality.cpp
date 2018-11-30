/*	 vitality.cpp
DATE Nov 09 2018
AUTH xazak
DESC This file implements the baseline statistics carried by every creature.
 */

#include <math.h>
#include "main.hpp"

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
void Vitality::burnCalories() {
	// Given an action's base caloric cost, modulate with player's bodyweight
	// and tenacity, then deduct from player's calorie bank
	// If needed, trigger any status changes due to hunger shifts
	// The baseline average, such that an adult tiger needs ~6 kg meat / day
	// (@4Kcal/g, ~24000 Kcal/day) is given by C(x) = 16x where x = seconds
}
