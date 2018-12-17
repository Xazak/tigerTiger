/*	 mortality.hpp
DATE Oct 25 2018
AUTH xazak
DESC Definitions of functions related to hit point adjustment and death.
 */
class Mortality {
	public:
		Mortality(float newMaxHP, float newDefense, const char *cadaverTitle);
		virtual ~Mortality();
		virtual void embraceDeath(Actor *subject); // corpse generation
		float adjustHP(Actor *subject, float hpAdjustment); // HP changes
		// **** QUERIES
		inline bool isDead() { return currentHP <= 0; }
		// **** TOOLS
		// these may need clarification, ie "this->maximumHP = newValue;"
		void setMaximumHP(float newValue) { maximumHP = newValue; }
		float getMaximumHP() { return maximumHP; }
		void setCurrentHP(float newValue) { currentHP = newValue; }
		float getCurrentHP() { return currentHP; }
		void setDefenseRating(float newValue) { defense = newValue; }
		float getDefenseRating() { return defense; }
		//fxns for load, save, and data-block creation will go here

		const char *cadaverTitle;

	protected:
		float maximumHP;
		float currentHP;
		float defense;
};
class PlayerMortality: public Mortality {
	public:
		PlayerMortality(float maximumHP, float defense, const char *cadaverTitle);
		void embraceDeath(Actor *subject);
		//also needs custom load/save routines
};
class NPCMortality: public Mortality {
	public:
		NPCMortality(float maximumHP, float defense, const char *cadaverTitle);
		void embraceDeath(Actor *subject);
		//load/save routines
};
