class Actor {
	public:
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
		Container *container; // ptr to internal-inventory handling
		Portable *portable; // ptr to external-inventory handling ???

		Actor(int inputX, int inputY, int sigil, const TCODColor &color, const char *name);
		~Actor();
		void update(); // ?
		void render() const; // draws the actor on the console
		float getDistance(int cx, int cy) const;
		//still needs load/save fxns -- TCODZip or Boost libraries?
};
