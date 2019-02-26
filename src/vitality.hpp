/*	 vitality.hpp
DATE Nov 09 2018
AUTH xazak
DESC Definitions of the baseline statistics carried by every creature.
 */
class Consumable {
	// defines the Consumable type, for use with anything that a creature might
	// ingest over the course of the game, including potions and food both
	public:
		Consumable(uint inputCalories = 0);
//		~Consumable();

		uint ingest(); // returns the caloric value and consumes the item
		void save(TCODZip &fileBuffer); // save to file
		void load(TCODZip &fileBuffer); // load from file
		void setCaloricValue(int newValue) { caloricValue = newValue; }
		uint getCaloricValue() { return caloricValue; }
	private:
		uint caloricValue; // how much is this worth in Kcal? (note: 0 is valid!)
};
class Vitality {
	public:
		// con/destructors
		Vitality();
//		~Vitality();
		void save(TCODZip &fileBuffer);
		void load(TCODZip &fileBuffer);
		// derived statistics
		uint getLeapDistance();
		uint getSprintSpeed();
		// tools
		void metabolize(); // general bookkeeping function
		void burnCalories(); // deducts calories according to (some) rate
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
		void setCurrentSatiety(uint newValue) { this->currSatiety = newValue; }
		uint getCurrentSatiety() { return this->currSatiety; }
		void setCurrentThirst(uint newValue) { this->currThirst = newValue; }
		uint getCurrentThirst() { return this->currThirst; }
		void setCurrentSleep(uint newValue) { this->currSleep = newValue; }
		uint getCurrentSleep() { return this->currSleep; }

	protected:
		uint ferocity; // burst-strength and power, damage rating
		uint tenacity; // endurance, willpower, focus
		uint swiftness; // speed, manual dexterity
		uint quiescence; // stillness, stealthiness
		uint perception; // acuity of sight and mind, wisdom
		uint tailLength;
		uint currSatiety; // high = well-fed, low = starving
		uint maxSatiety;
		uint currThirst; // high = quenched, low = thirsty
		uint maxThirst;
		uint currSleep; // high = awake, low = narcoleptic
		uint maxSleep;
};
class PlayerVitality: public Vitality {
	public:

};
class AnimalVitality: public Vitality {
	public:

};
