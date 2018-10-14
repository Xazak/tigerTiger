static const int SCENT_THRESHOLD=20;

class Ai {
    public:
        virtual void update(Actor *owner) = 0;
        virtual ~Ai() {};
};
class PlayerAi : public Ai {
    public:
        void update(Actor *owner);
        void handleActionKey(Actor *owner, int ascii);

    protected:
        bool moveOrAttack(Actor *owner, int targetx, int targety);
        Actor *choseFromInventory(Actor *owner);
};
class MonsterAi : public Ai {
    public:
        void update(Actor *owner);

    protected:
        void moveOrAttack(Actor *owner, int targetx, int targety);
};
class ConfusedMonsterAi : public Ai {
    public:
        ConfusedMonsterAi(int nbTurns, Ai *oldAi);
        void update(Actor *owner);

    protected:
        int nbTurns;
        Ai *oldAi;
};
