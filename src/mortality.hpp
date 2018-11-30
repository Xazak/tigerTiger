/*	 mortality.hpp
DATE Oct 25 2018
AUTH xazak
DESC Definitions of functions related to hit point adjustment and death.
 */
class Mortality {
	public:
		const char *cadaverTitle;

		Mortality(float newMaxHP, float newDefense, const char *cadaverTitle);
		virtual ~Mortality();
		inline bool isDead() { return currentHP <= 0; }
		virtual void embraceDeath(Actor *subject);
		float adjustHP(Actor *subject, float hpAdjustment);
		// simple sets/gets
		void setMaximumHP(float newMaxHP);
		float getMaximumHP();
		void setCurrentHP(float hpChange);
		float getCurrentHP();
		void setDefenseRating(float newRating);
		float getDefenseRating();
		//fxns for load, save, and data-block creation will go here
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
