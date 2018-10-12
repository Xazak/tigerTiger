class Actor { 
public:
    int x, y; // position on map
    int ch; // ascii symbol code
        // note: a numeric value is used instead of a string literal so that 
        // symbols located outside the 'normal' range can be used
    TCODColor col; // symbol color
    const char *name; // actor's name
    bool blocks; // does this actor block movement?
    Attacker *attacker; // something that deals damage
    Destructible *destructible; // something that can be damaged
    Ai *ai; // something sentient
    Pickable *pickable; // something that can be picked up and used
    Container *container; // something that can contain actors

    Actor(int x, int y, int ch, const char *name, const TCODColor &col);
    ~Actor();
    void update();
    void render() const;
};
