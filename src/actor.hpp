/*	 actor.hpp
DATE Oct 08 2018
AUTH xazak
DESC Definitions of Actor class, the prototype for all game objects, and assoc.
 */
class Actor {
	public:
		Actor(int inputX, int inputY, int sigil, const TCODColor &color, const char *name);
		~Actor();
		bool update(); // tells a sentient actor to make a decision/change state
		void render() const; // draws the actor on the console
		float getDistance(int cx, int cy) const;
		void save(TCODZip &fileBuffer); // save to file
		void load(TCODZip &fileBuffer); // load from file

		int xpos, ypos; // position on map
		int sigil; // code number for actor's map sigil
			// NOTE: numeric value is used here instead of a string so that we
			// can invoke extended symbols in the ASCII standard
		TCODColor color; // sigil color
		const char *name; // actor's name
		bool obstructs; // true = cannot be moved through
		//Attack
		Sentience *sentience; // ptr to AI subroutines
		Mortality *mortality; // ptr to HP & death subroutines
		ActorClock *tempo; // ptr to AP system
		Container *container; // ptr to actor's inventory
		Portable *portable; // ptr to allow this actor to be picked up
};
