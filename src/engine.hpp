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
		void mainMenu();
		void saveToFile();
		void loadFromFile();
		void exitGame();
		void deleteSavedGame();
		// pushes the *actor to the rear-most screen layer so that other sigils
		// can be drawn on top of it, ie for non-blocking actors
		void sendToBack(Actor *actor); // causes other actors to draw above it
		// looks through allActors for closest ACTOR
		Actor *getClosestMonster(int x, int y, float range) const;
		// asks player to specify a tile; default is self?
		bool pickATile(int *x, int *y, float maxRange = 0.0f);
		// iterates through allActors to find the actor at the given coords
		// there's gotta be a better way to get this information...
		Actor *getActor(int x, int y) const;
		void updateActionQueue(); // populates the actionQueue with ACTORS
		void refreshAP(); // iterates through actionQueue and grants AP
		void switchMode(EngineState newMode); // handles engine state changes

		EngineState currMode; // these two hold the running state of the engine
		EngineState prevMode; // and the previous state in case we need it

		GameGUI *gui; // points to master GUI object
		WorldClock *chrono; // points to world-scope timekeeping tools
		GameMap *map; // points to master map object
		TCODRandom *rng; // pointer to RNG engine
		TCODList<Actor *> allActors; // contains ALL ACTORS
		TCODList<Actor *> actionQueue; // contains only LOCAL and SENTIENT actors
		Actor *currActor; // points to the actor that is currently updating
		Actor *player; // always points to player actor

		TCOD_key_t lastKey; // contains last key pressed by player
		TCOD_mouse_t mouse; // contains last mouse press by player

		int screenWidth = 0; // width of window containing root terminal
		int screenHeight = 0; // height of window containing root terminal
		int fovRadius; // player-actor's sight radius (move to Actor?)

	private:
//		FileInterface scribe; // handles save/load requests
		long seed; // RNG seed (stored for reuse)
};
extern GameEngine engine; // 'extern' tells the compiler to look elsewhere for the actual declaration
