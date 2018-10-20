#include <stdio.h> //?
#include <stdarg.h> //?
#include "main.hpp"

static const int PANEL_HEIGHT = 7; // height of GUI console
static const int BAR_WIDTH = 20; // width of health bar
static const int MSG_X = BAR_WIDTH + 2; // x-position of message log
static const int LOG_MAX_SIZE = PANEL_HEIGHT - 1; // max lines in message log

Gui::Gui() {
	guiCon = new TCODConsole(engine.screenWidth, PANEL_HEIGHT);
}
Gui::~Gui() {
	delete guiCon;
	clear();
}
void Gui::render() {
	// clear the console
	guiCon->setDefaultBackground(TCODColor::black);
	guiCon->clear();
	// draw the health bar
	renderBar(1, 1, BAR_WIDTH, "HP", 69.0f, 70.0f,
		TCODColor::lightRed, TCODColor::darkerRed);
	// draw the message log
	int y = 1;
	float colorCoef = 0.4f;
	for (Message **iter = log.begin(); iter != log.end(); iter++) {
		Message *logLine = *iter;
		guiCon->setDefaultForeground(logLine->color * colorCoef);
		guiCon->printf(MSG_X, y, logLine->msgText);
		y++;
		if (colorCoef < 1.0f) {
			colorCoef += 0.3f;
		}
	}
//	renderMouseLook();
	// blit the GUI console onto the root console
	// OPTIONS: source console, x and y positions, width and height,
	//          dest. console, x and y positions, fore/background alpha values
	TCODConsole::blit(guiCon, 0, 0, engine.screenWidth, PANEL_HEIGHT,
		TCODConsole::root, 0, engine.screenHeight - PANEL_HEIGHT);
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
		if (log.size() == LOG_MAX_SIZE) { // are we near full?
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
void Gui::renderBar(int x, int y, int width, const char *name, float curValue,
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
}
Gui::Message::Message(const char *inputText, const TCODColor &color):
	msgText(strdup(inputText)), color(color) { }
Gui::Message::~Message() {
	free(msgText);
}
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
		for (MenuItem **iter = menuListItems.begin();
				iter != menuListItems.end(); iter++) {
			// recolor menu text to act as cursor
			if (currentItem == selectedItem) {
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			} else {
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
