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
		Menu menu;

		Gui();
		~Gui();
		void render(); // draw the GUI on the screen
		void message(const TCODColor &color, const char *text, ...); //?
		void clear(); // wipe the message log
//		void renderMouseLook();
		// need load and save fxns
	protected:
		TCODConsole *guiCon; // pointer to the GUI console
		// draw a two-color bar on the screen (ie a health bar)

		void renderBar(int x, int y, int width, int height, const char *name,
			float curValue, float maxValue, const TCODColor &foreColor,
			const TCODColor &backColor);

		struct Message {
			char *msgText;
			TCODColor color;
			Message(const char *inputText, const TCODColor &color);
			~Message();
		};
		TCODList<Message *> log; // pointer to the message log
};
