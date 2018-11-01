/*FILE: gui.cpp
  Implements the methods and routines for drawing and communicating with the
  game interface, as well as the viewport logic and organization.
  Keyboard bindings are currently located in sentience.cpp::handleActionInput().
  */
/*METHOD
  The entire screen shall be broken up into three panels:
  1) the viewport (ie camera) onto the terrain map
  2) a panel of statistics and certain UI elements
  3) a message log
  This shall be organized like so:
  +------+-+
  |      | |
  |  1   |2|
  +------+ |
  |  3   | |
  +------+-+
  (1) starts from (0, 0); width = 75% of screenWidth; height = screenHeight - (3).height
  (2) starts from the UR corner of (1); 20 <= width <= 25% of screenWidth; height = screenHeight
  (3) starts from the BL corner of (1); width = (1).width; 1 <= height <= 10% of screenHeight

  These should be built up in such a way that their positions can be rearranged

  */
#include <stdarg.h>
#include "main.hpp"

static const TCODColor GUI_FORE = TCODColor::silver;
static const TCODColor GUI_BACK = TCODColor::black;

Gui::Gui() {
	// change the initializations depending on layout...
//	viewport = new GuiPane(0, 0, engine.screenWidth - (engine.screenWidth / 4), engine.screenHeight - 6);
//	int msgPanelHeight = 6;
//	int viewportWidth = engine.screenWidth - (engine.screenWidth / 4);
//	int viewportHeight = engine.screenHeight - msgPanelHeight;
	viewport = new TCODConsole(
		(engine.screenWidth - (engine.screenWidth / 4)),
		engine.screenHeight - 6);
	statPanelXPos = viewport->getWidth();
	statPanelYPos = 0;
	msgPanelXPos = 0;
	msgPanelYPos = viewport->getHeight();
//	statPanel = new TCODConsole((engine.screenWidth / 4), engine.screenHeight);
//	msgPanel = new TCODConsole(viewport->getWidth(), 6);
//	statPanel= new GuiPane((viewport->getWidth()), 0, engine.screenWidth / 4,
//		engine.screenHeight);
//	msgPanel = new GuiPane(0, (viewport->getHeight()), viewport->getWidth(), 6);
//	logSize = (msgPanel->getHeight() - 1);
}
Gui::~Gui() {
	delete viewport;
//	delete statPanel;
//	delete msgPanel;
	clear();
}
void Gui::render() {
	// draw the viewport
	// panel borders
	viewport->setDefaultBackground(GUI_BACK);
	viewport->setDefaultForeground(GUI_FORE);
	LOGMSG("Viewport cleared");
	viewport->clear();
	// panel contents
	TCODConsole::blit(viewport, 0, 0, viewport->getWidth(), viewport->getHeight(), TCODConsole::root,
		viewport->getWidth(), viewport->getHeight());

	// draw the stat panel
	// panel borders
//	statPanel->setDefaultBackground(TCODColor::darkestBlue);
//	statPanel->setDefaultForeground(GUI_FORE);
//	statPanel->clear();
//	statPanel->vline(0, 0, statPanel->getHeight());
	// panel contents
//	TCODConsole::blit(statPanel, 0, 0, statPanel->getWidth(), statPanel->getHeight(), TCODConsole::root,
//		statPanelXPos, statPanelYPos);

	// draw the message log
	// panel borders
//	msgPanel->setDefaultBackground(TCODColor::darkestGreen);
//	msgPanel->setDefaultForeground(GUI_FORE);
//	msgPanel->clear();
//	msgPanel->hline(0, 0, msgPanel->getWidth());
	// panel contents
//	TCODConsole::blit(msgPanel, 0, 0, msgPanel->getWidth(), msgPanel->getHeight(), TCODConsole::root,
//		msgPanelXPos, msgPanelYPos);

//	blitToScreen();
}
/*void Gui::blitToScreen() {
//	LOGMSG(" has been called\n");
	TCODConsole::blit(viewport, 0, 0, viewport->getWidth(), viewport->getHeight(), TCODConsole::root,
		viewport->getWidth(), viewport->getHeight());
	TCODConsole::blit(statPanel, 0, 0, 0, 0, TCODConsole::root,
		statPanel->getWidth(), statPanel->getHeight());
	TCODConsole::blit(msgPanel, 0, 0, 0, 0, TCODConsole::root,
		msgPanel->getWidth(), msgPanel->getHeight());
}*/
void Gui::renderTile(int inputx, int inputy, int newSigil, const TCODColor foreColor,
	const TCODColor backColor) {
//	LOGMSG("Placing " << (char)newSigil << " at " << inputx << ", " << inputy);
//	viewport->con->putCharEx(inputx, inputy, newSigil, foreColor, backColor);
//	TCODConsole::blit(viewport->con, 0, 0, 0, 0, TCODConsole::root,
//		viewport->xpos, viewport->ypos);
}
void Gui::message(const TCODColor &color, const char *msgText, ...) {
	// this fxn needs better annotation!
	va_list ap; //?
	char buffer[128];
	va_start(ap, msgText);
	vsprintf(buffer, msgText, ap);
	va_end(ap);
	char *lineBegin = buffer;
	char *lineEnd;
	do {
		// make room for the new message
		if (log.size() == Gui::logSize) { // are we near full?
			Message *targetLine = log.get(0); // get the oldest line
			log.remove(targetLine); // remove from the log
			delete targetLine; // delete the message from memory
		}
		// strip EOL chars from the message text
		lineEnd = strchr(lineBegin, '\n');
		if (lineEnd) {
			*lineEnd = '\0';
		}
		// add the message to the log
		Message *newMsg = new Message(lineBegin, color);
		log.push(newMsg);
		// move the front of the log forward
		lineBegin = lineEnd + 1;
	} while (lineEnd);
}
void Gui::clear() {
	log.clearAndDelete(); // wipe the message log
}
/*GuiPane::GuiPane(int inputx, int inputy, int inputw, int inputh):
	xpos(inputx), ypos(inputy), width(inputw), height(inputh) {
	con = new TCODConsole(width, height);
	LOGMSG("New panel created at (" << xpos << ", " << ypos << "), " << width << "x" << height);
}
GuiPane::~GuiPane() {
	delete con;
}*/
/*void Gui::renderBar(int x, int y, int width, const char *name, float curValue,
	float maxValue, const TCODColor &foreColor, const TCODColor &backColor) {
	// draw the background
	guiCon->setDefaultBackground(backColor);
	guiCon->rect(x, y, width, 1, false, TCOD_BKGND_SET);
	int barWidth = (int)(curValue / maxValue * width);
	if (barWidth > 0) {
		// only draw the bar if there's enough bar worth drawing
		guiCon->setDefaultBackground(foreColor);
		guiCon->rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
	}
	// print the text on top of the bar
	guiCon->setDefaultForeground(TCODColor::white); // set text color
	// draw the text: xy coords, backgrnd color, justification, [printf()]
	guiCon->printf((x + width / 2), y, TCOD_BKGND_NONE, TCOD_CENTER,
		"%s : %g/%g", name, curValue, maxValue);
}*/
// *** MESSAGES
Gui::Message::Message(const char *inputText, const TCODColor &color):
	msgText(strdup(inputText)), color(color) { }
Gui::Message::~Message() { free(msgText); }
// *** MENU
Menu::~Menu() {
	clear();
}
void Menu::clear() {
	menuListItems.clearAndDelete();
}
void Menu::addItem(MenuItemCode code, const char *label) {
	MenuItem *item = new MenuItem();
	item->code = code;
	item->label = label;
	menuListItems.push(item);
}
Menu::MenuItemCode Menu::pick() {
//	static TCODImage img("menu_background1.png"); // main menu background image
	int selectedItem = 0;
	while (!TCODConsole::isWindowClosed()) {
//		img.blit2x(TCODConsole::root, 0, 0); // blit the bkgrnd img on screen
		int currentItem = 0;
		// we're going to let the menu blit to the root console for now
		for (MenuItem **iter = menuListItems.begin();
				iter != menuListItems.end(); iter++) {
			// recolor menu text to act as cursor
			if (currentItem == selectedItem) {
//				engine.gui->viewport->setDefaultForeground(TCODColor::lighterOrange);
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			} else {
//				engine.gui->viewport->setDefaultForeground(TCODColor::lightGrey);
				TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
			}
			TCODConsole::root->printf(10, 10 + currentItem * 3, (*iter)->label);
			currentItem++;
		}
		TCODConsole::flush(); //?
		// handle menu selection keypresses
		TCOD_key_t key;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
		switch (key.vk) {
			case TCODK_UP:
				selectedItem--;
				if (selectedItem < 0) {
					selectedItem = menuListItems.size() - 1;
				}
				break;
			case TCODK_DOWN:
				selectedItem = (selectedItem + 1) % menuListItems.size();
				break;
			case TCODK_ENTER:
				return menuListItems.get(selectedItem)->code;
			default:
				break;
		}
	}
	return NONE;
}
