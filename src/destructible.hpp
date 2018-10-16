class Destructible {
	public:
		float maxHP;
		float curHP;
		float defense;
		const char *corpseName; // the actor's name when they have ceased to be

		Destructible(float maxHP, float defense, const char *corpseName);
		virtual ~Destructible();
		inline bool isDead() {return curHP <= 0;}
		float takeDamage(Actor *owner, float damage); // damage given to Destructible
		float heal(float amount); // healing given to Destructible
		virtual void die(Actor *owner);
		void load(TCODZip &zip);
		void save(TCODZip &zip);
		static Destructible *create(TCODZip &zip);

	protected:
		enum DestructibleType {
			MONSTER, PLAYER
		};
};
class MonsterDestructible : public Destructible {
	public:
		MonsterDestructible(float maxHP, float defense, const char *corpseName);
		void die(Actor *owner);
		void load(TCODZip &zip);
		void save(TCODZip &zip);
};
class PlayerDestructible : public Destructible {
	public:
		PlayerDestructible(float maxHP, float defense, const char *corpseName);
		void die(Actor *owner);
		void load(TCODZip &zip);
		void save(TCODZip &zip);
};

