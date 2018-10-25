/*FILE: sentience.hpp
  Contains action packages that provide decision-making tools for Actors.
  */
class Sentience {
	public:
		virtual void update(Actor *subject) = 0;
	protected:
		enum SentienceType {
			PLAYER, PREY, PREDATOR, HUMAN
		};
};
class PlayerSentience: public Sentience {
	public:
		void update(Actor *subject);
	protected:
		void handleActionInput(Actor *subject, int inputKeystroke);
		bool decideMoveAttack(Actor *subject, int targetx, int targety);
		Actor *choseFromInventory(Actor *subject);
};
class AnimalSentience: public Sentience {
	public:
		void update(Actor *subject);
	protected:
		bool decideMoveAttack(Actor *subject, int targetx, int targety);
};
