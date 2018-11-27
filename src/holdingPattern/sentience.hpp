/*FILE: sentience.hpp
  Contains action packages that provide decision-making tools for Actors.
  */
class Sentience {
	public:
		virtual bool update(Actor *subject) = 0;
		virtual int getCheapestActionCost() = 0;
	protected:
		enum SentienceType {
			PLAYER, PREY, PREDATOR, HUMAN
		};
};
class PlayerSentience: public Sentience {
	public:
		bool update(Actor *subject);
		int getCheapestActionCost();
	protected:
		bool handleActionInput(Actor *subject, int inputKeystroke);
		bool decideMoveAttack(Actor *subject, int targetx, int targety);
		Actor *chooseFromInventory(Actor *subject);
};
class AnimalSentience: public Sentience {
	public:
		bool update(Actor *subject);
		int getCheapestActionCost();
	protected:
		bool decideMoveAttack(Actor *subject, int targetx, int targety);
};
