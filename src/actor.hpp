class Actor { 
public:
    int x, y; // position on map
    int ch; // ascii symbol code
        // note: a numeric value is used instead of a string literal so that 
        // symbols located outside the 'normal' range can be used
    TCODColor col; // symbol color
    const char *name; // actor's name
    char name [MAX_NAME_LENGTH];

    Actor(int x, int y, int ch, const char *name, const TCODColor &col);
    void update();
    bool moveOrAttack(int x, int y);
};
