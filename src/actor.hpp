/*	 actor.hpp
DATE Oct 08 2018
AUTH xazak
DESC Definitions of Actor class, the prototype for all game objects, and assoc.
 */
class Actor {
	public:
		// new Actor(xpos, ypos, symbol, color, name);
		Actor(int inputX, int inputY, int sigil, const TCODColor &color, const char *name);
		Actor(TCODZip &fileBuffer, bool isPlayer = false);
		~Actor();
		bool update(); // tells a sentient actor to make a decision/change state
		void render() const; // draws the actor on the console
		float getDistance(int cx, int cy) const;
		void save(TCODZip &fileBuffer); // save to file
		void load(TCODZip &fileBuffer, bool isPlayer = false); // load from file

		int xpos, ypos; // position on map
		int sigil; // code number for actor's map sigil
			// NOTE: numeric value is used here instead of a string so that we
			// can invoke extended symbols in the ASCII standard
		const char *name; // actor's name
		bool obstructs; // true = cannot be moved through
		TCODColor color; // sigil color
		Sentience *sentience; // ptr to AI subroutines
		Vitality *vitality; // ptr to core stats and skill check bonuses
		Mortality *mortality; // ptr to HP & death subroutines
//		Violence *violent; // ptr to combat subroutines
		ActorClock *tempo; // ptr to AP system
		Container *container; // ptr to actor's inventory
		Portable *portable; // ptr to allow this actor to be picked up

		// *** INSTANTIATION/SPAWN TOOLS
		// **** LOOK IN CREATURES.CPP FOR THESE FXNS
		void spawnCreature(std::string type, int x, int y);
		void newLangurMonkey(int x, int y);
};
