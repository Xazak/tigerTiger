/*	 gui.hpp
DATE Oct 12 2018
AUTH xazak
DESC Definitions of Gui and Menu classes
 */
class Menu {
	public:
		enum MenuItemCode { // contents of main menu
			NONE, NEW_GAME, CONTINUE, SAVE, QUIT
		};
		~Menu();
		void clear(); // clear the menu
		void addItem(MenuItemCode code, const char *label); // add to the menu
		MenuItemCode pick(); // pick an item from the menu
	protected:
		struct MenuItem {
			MenuItemCode code;
			const char *label;
		};
		TCODList<MenuItem *> menuListItems; // pointer to list of menu items
};
class GameGUI {
	public:
		GameGUI();
		~GameGUI();
		void save(TCODZip &fileBuffer);
		void load(TCODZip &fileBuffer);
		void blitToScreen(); // draw all GUI panels on the root console
		void render(); // draw the GUI on the screen
		// for drawing a single tile? need to check...
		void renderTile(int inputx, int inputy, int newSigil,
			const TCODColor foreColor, const TCODColor backColor);
		void refreshViewport(); // updates the viewport edge/offset coordinates
		void refreshScrollingEdges(); // updates the far viewport boundaries
//		void renderMouseLook();
		// MESSAGE LOG TOOLS
		// print a message to the log
		void message(const TCODColor &color, const char *text, ...);
		void clear(); // wipe the message log

		bool updateView = false; // if true, invoke render()
		Menu menu; // basic menu object
		TCODConsole *viewport; // viewport console
		TCODConsole *statPanel; // statpanel console
		TCODConsole *msgPanel; // message log console
		// the half-sizes of the viewport's width and height
		int viewportXOffset;
		int viewportYOffset;
		// the viewport's x,y origin within the map array
		int viewportXOrigin;
		int viewportYOrigin;
		// the far edges of the map, where we should not scroll the viewport
		int farHorizEdge; // bottom side of screen
		int farVertEdge; // right side of screen
		// x, y positions relative to viewport w/in entire GUI space
		int statPanelXPos;
		int statPanelYPos;
		int msgPanelXPos;
		int msgPanelYPos;

	protected:
		int logSize; // length of message log in (wrapped) lines
		// draw a two-color bar on the screen (ie a health bar)
		void renderBar(int x, int y, int width, const char *name,
			float value, float maxValue, const TCODColor &barColor,
			const TCODColor &backColor);
		// STAT PANEL TOOLS
		// print the player's HP, defense, core stats
		// returns the number of lines it took up printing its info
		int displayVitals(TCODConsole *console, int height);
		// draws a stat panel with debug info
		// returns the number of lines it took up printing its info
		int debugStats(TCODConsole *console, int height);
		// simple object that populates the message log
		struct Message {
			char *msgText;
			TCODColor color;
			Message(const char *inputText, const TCODColor &color);
			~Message();
		};
		TCODList<Message *> log; // pointer to the message log
};
