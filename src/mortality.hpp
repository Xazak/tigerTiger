/*FILE: mortality.hpp
  Contains definitions of functions related to hit point adjustment and death.
  */
class Mortality {
	public:
		float maximumHP;
		float currentHP;
		float defense;
		const char *cadaverTitle;

		Mortality(float newMaxHP, float newDefense, const char *cadaverTitle);
		virtual ~Mortality();
		inline bool isDead() { return currentHP <= 0; }
		virtual void embraceDeath(Actor *subject);
		float adjustHP(Actor *subject, float hpAdjustment);
		//fxns for load, save, and data-block creation go here
	protected:
		enum MortalityType { PLAYER, NPC, OBJECT };
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
