class EngineState {
	public:
		virtual ~EngineState() { };
		virtual void update();

//		static StartupTurn startup;
//		static NewTurn newTurn;
//		static OngoingTurn ongoingTurn;
//		static IdleMode idleMode;
//		static VictoryMode victoryMode;
//		static DefeatMode defeatMode;
};
class StartupTurn: public EngineState {
	public:
		~StartupTurn() { };
		void update();
} static startupTurn;
class NewTurn: public EngineState {
	public:
		~NewTurn() { };
		void update();
};
class OngoingTurn: public EngineState {
	public:
		~OngoingTurn() { };
		void update();
};
class IdleMode: public EngineState {
	public:
		~IdleMode() { };
		void update();
};
class VictoryMode: public EngineState {
	public:
		~VictoryMode() { };
		void update();
};
class DefeatMode: public EngineState {
	public:
		~DefeatMode() { };
		void update();
};
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
		EngineState *mode;

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
		void update(); //?
		void render(); //?
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
