class Engine {
    public:
        enum GameStatus {
            STARTUP,
            IDLE,
            NEW_TURN,
            VICTORY,
            DEFEAT
        } gameStatus;
        TCODList<Actor *> actors;   // the list of all actors on the map
        Actor *player;              // ptr to the player object
        Map *map;                   // ptr to the master Map object
        int fovRadius;
        int screenWidth;
        int screenHeight;
        Gui *gui;
        TCOD_key_t lastKey;
        TCOD_mouse_t mouse;

        Engine(int screenWidth, int screenHeight);
        ~Engine();
        void update();
        void render();
        void sendToBack(Actor *actor);

    private:
//        bool computeFOV; //replaced when adding the enum above...
};

extern Engine engine; // 'extern' tells compiler that the global var 'engine' has been declared elsewhere
