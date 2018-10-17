class Attacker {
	public:
		float power; // damage given

		Attacker(float power);
		void attack(Actor *owner, Actor *target);
		void load(TCODZip &zip);
		void save(TCODZip &zip);
};
