class EngineState {
	public:
		virtual void update() {};
	private:
};
class StartupTurn: public EngineState {
	public:
		void update();
} static startupTurn;
class NewTurn: public EngineState {
	public:
		void update();
} static newTurn;
class OngoingTurn: public EngineState {
	public:
		void update();
} static ongoingTurn;
class IdleMode: public EngineState {
	public:
		void update();
} static idleMode;
class VictoryMode: public EngineState {
	public:
		void update();
} static victory;
class DefeatMode: public EngineState {
	public:
		void update();
} static defeat;
class Engine {
	public:
		// changing gameStatus tells the engine to change states
		enum GameStatus {
			STARTUP,		//0
			IDLE,			//1
			NEW_TURN,		//2
			ONGOING_TURN,	//3
			VICTORY,		//4
			DEFEAT			//5
		} gameStatus;
		// changing engine.currMode is how individual game turns are processed
		// engine.prevMode is included for later troubleshooting needs
		EngineState *currMode;
		EngineState *prevMode;
		void switchMode(EngineState *newMode);

		TCODList<Actor *> actors; // the list of all actors on the map
		Actor *player; // ptr to the player object
		Map *map; // ptr to the master Map object
		int fovRadius; // should this come from the actor object?
		int screenWidth;
		int screenHeight;
		Gui *gui; // pointer to main GUI object
		GameClock *time; // pointer to calendar/scheduler tools
		TCOD_key_t lastKey; // contains last key pressed by player
//		TCOD_mouse_t mouse;

		Engine(int screenWidth, int screenHeight);
		~Engine();
		void update(); // invokes update depending on engineState
		void render(); // invokes full panoply of rendering function calls
		// pushes the *actor to the rear-most screen layer so that other sigils
		// can be drawn on top of it, ie for non-blocking actors
		void sendToBack(Actor *actor);
		Actor *getClosestMonster(int x, int y, float range) const;
		bool pickATile(int *x, int *y, float maxRange = 0.0f);
		Actor *getActor(int x, int y) const;
		void init(); // starts the game engine
		void term(); // stops the game engine
		void save();
		void load();
};

extern Engine engine; // 'extern' tells compiler that the global var 'engine' has been declared elsewhere
