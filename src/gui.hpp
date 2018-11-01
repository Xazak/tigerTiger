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
struct GuiPane {
	TCODConsole *con;
	int xpos;
	int ypos;
	int width;
	int height;
	GuiPane(int inputx, int inputy, int inputw, int inputh);
	~GuiPane();
};
class Gui {
	public:
		bool updateView = false; // if true, invoke render()
		Menu menu;
		TCODConsole *viewport;
		TCODConsole *statPanel;
		TCODConsole *msgPanel;
		int statPanelXPos;
		int statPanelYPos;
		int msgPanelXPos;
		int msgPanelYPos;
//		GuiPane *viewport;
//		GuiPane *statPanel;
//		GuiPane *msgPanel;

		Gui();
		~Gui();
		void render(); // draw the GUI on the screen
		void renderTile(int inputx, int inputy, int newSigil,
			const TCODColor foreColor, const TCODColor backColor); // updates viewport to match maps
		// print a message to the log
		void message(const TCODColor &color, const char *text, ...);
		void clear(); // wipe the message log
		void blitToScreen(); // draw all GUI panels on the root console
//		void renderMouseLook();
		// need load and save fxns
	protected:
		// draw a two-color bar on the screen (ie a health bar)
		int logSize;
		void renderBar(int x, int y, int width, const char *name,
			float value, float maxValue, const TCODColor &barColor,
			const TCODColor &backColor);
		struct Message {
			char *msgText;
			TCODColor color;
			Message(const char *inputText, const TCODColor &color);
			~Message();
		};
		TCODList<Message *> log; // pointer to the message log
};
