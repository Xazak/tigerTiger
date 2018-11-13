class Menu {
	public:
		enum MenuItemCode { // contents of main menu
			NONE, NEW_GAME, CONTINUE, EXIT
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
class Gui {
	public:
		bool updateView = false; // if true, invoke render()
		Menu menu;
		TCODConsole *viewport;
		TCODConsole *statPanel;
		TCODConsole *msgPanel;
		// the half-sizes of the viewport's width and height
		int viewportXOffset;
		int viewportYOffset;
		// the viewport's x,y origin within the map array
		int viewportXOrigin;
		int viewportYOrigin;
		// these hold the x,y positions within the terminal itself
		int statPanelXPos;
		int statPanelYPos;
		int msgPanelXPos;
		int msgPanelYPos;

		Gui();
		~Gui();
		void blitToScreen(); // draw all GUI panels on the root console
		void render(); // draw the GUI on the screen
		void renderTile(int inputx, int inputy, int newSigil,
			const TCODColor foreColor, const TCODColor backColor); // updates viewport to match maps
		void refreshViewport();
		// print a message to the log
		void message(const TCODColor &color, const char *text, ...);
		void clear(); // wipe the message log
//		void renderMouseLook();
		// need load and save fxns
	protected:
		// draw a two-color bar on the screen (ie a health bar)
		int logSize;
		void renderBar(int x, int y, int width, const char *name,
			float value, float maxValue, const TCODColor &barColor,
			const TCODColor &backColor);
		// simple object for use with the message log
		struct Message {
			char *msgText;
			TCODColor color;
			Message(const char *inputText, const TCODColor &color);
			~Message();
		};
		TCODList<Message *> log; // pointer to the message log
};
