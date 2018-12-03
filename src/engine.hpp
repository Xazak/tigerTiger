/*	 engine.hpp
DATE Oct 08 2018
AUTH xazak
DESC Defintions of the game engine, which handles update and render calls.
 */
class GameEngine {
	public:
		enum EngineState {
			STARTUP,	//0
			IDLE,		//1
			ONGOING,	//2
			NEWTURN,	//3
			VICTORY,	//4
			DEFEAT		//5
		};
		void switchMode(EngineState newMode);
		EngineState currMode;
		EngineState prevMode;

		GameGUI *gui; // points to master GUI object
		int screenWidth = 0;
		int screenHeight = 0;

//		GameClock *chrono; // points to world-scope timekeeping tools

		TCODList<Actor *> allActors; // contains ALL ACTORS
		TCODList<Actor *> actionQueue; // contains only LOCAL and SENTIENT actors
		Actor *currActor; // points to the actor that is currently updating
		Actor *player; // always points to player actor
		TCOD_key_t lastKey; // contains last key pressed by player
	//	TCOD_mouse_t mouse;
		int fovRadius;

		GameMap *map; // points to master map object

		// *** MAJOR FXNS
		GameEngine(int screenWidth, int screenHeight);
		~GameEngine();
		void init(); // starts the game engine
		void term(); // stops the game engine
		void save(); // write game state to external file
		void load(); // load game state from external file
		void update(); // processes game state updates
		void render(); // invokes lower-level render requests
		// *** MINOR FXNS
		// pushes the *actor to the rear-most screen layer so that other sigils
		// can be drawn on top of it, ie for non-blocking actors
		void sendToBack(Actor *actor);
		Actor *getClosestMonster(int x, int y, float range) const;
		bool pickATile(int *x, int *y, float maxRange = 0.0f);
		Actor *getActor(int x, int y) const;
		void updateActionQueue();
		void refreshAP();
		TCODRandom *rng; // pointer to RNG engine

	private:
		long seed; // RNG seed (stored for reuse)
};
extern GameEngine engine; // 'extern' tells the compiler to look elsewhere for the actual declaration
