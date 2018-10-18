class Actor {
	public:
		int actorX, actorY; // position on map
		int sigil; // code number for actor's map sigil
			// NOTE: numeric value is used here instead of a string so that we
			// can invoke extended symbols in the ASCII standard
		TCODColor color; // sigil color
		const char *name; // actor's name
		bool blockMovement; // true = cannot be moved through

		Actor(int inputX, int inputY, int sigil, const TCODColor &color, const char *name);
		~Actor();
		void update(); // ?
		void render() const; // draws the actor on the console
		float getDistance(int cx, int cy) const;
		//still needs load/save fxns -- TCODZip or Boost libraries?
};
