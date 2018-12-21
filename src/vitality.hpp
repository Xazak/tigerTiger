/*	 vitality.hpp
DATE Nov 09 2018
AUTH xazak
DESC Definitions of the baseline statistics carried by every creature.
 */
class Vitality {
	public:

		// con/destructors?
		void save(TCODZip &fileBuffer);
		void load(TCODZip &fileBuffer);
		// derived statistics
		uint getLeapDistance();
		uint getSprintSpeed();
		// tools
		void burnCalories(); // called at end of each player action
		void gainEnergy(uint calories); // called when player eats food
		// sets/gets
		void setFerocity(uint newValue) { this->ferocity = newValue; }
		uint getFerocity() { return this->ferocity; }
		void setTenacity(uint newValue) { this->tenacity = newValue; }
		uint getTenacity() { return this->tenacity; }
		void setSwiftness(uint newValue) { this->swiftness = newValue; }
		uint getSwiftness() { return this->swiftness; }
		void setQuiescence(uint newValue) { this->quiescence = newValue; }
		uint getQuiescence() { return this->quiescence; }
		void setPerception(uint newValue) { this->perception = newValue; }
		uint getPerception() { return this->perception; }
		void setSatiety(uint newValue) { this->satiety = newValue; }
		uint getSatiety() { return this->satiety; }

	protected:
		uint ferocity; // burst-strength and power, damage rating
		uint tenacity; // endurance, willpower, focus
		uint swiftness; // speed, manual dexterity
		uint quiescence; // stillness, stealthiness
		uint perception; // acuity of sight and mind, wisdom
		uint satiety; // high = well-fed, low = starving
};
class PlayerVitality: public Vitality {
	public:

};
class AnimalVitality: public Vitality {
	public:

};
