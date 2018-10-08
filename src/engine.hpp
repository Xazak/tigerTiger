class Engine {
    public:
        TCODList<Actor *> actors;   // the list of all actors on the map
        Actor *player;              // ptr to the player object
        Map *map;                   // ptr to the master Map object

        Engine();
        ~Engine();
        void update();
        void render();
};

extern Engine engine; // 'extern' tells compiler that the global var 'engine' has been declared elsewhere
