class Attacker {
    public:
        float power; // damage given

        Attacker(float power);
        void attack(Actor *owner, Actor *target);
};
