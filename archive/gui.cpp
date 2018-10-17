#include <stdio.h>
#include <stdarg.h>
#include "main.hpp"

static const int PANEL_HEIGHT = 7;
static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH + 2;
static const int MSG_HEIGHT = PANEL_HEIGHT - 1;

Gui::Gui() {
	con = new TCODConsole(engine.screenWidth, PANEL_HEIGHT);
}
Gui::~Gui() {
	delete con;
	clear();
}
void Gui::clear(){
	log.clearAndDelete();
}
void Gui::render() {
	// clear the console
	con->setDefaultBackground(TCODColor::black);
	con->clear();
	// draw the health bar
	renderBar(1, 1, BAR_WIDTH, "HP", engine.player->destructible->curHP, engine.player->destructible->maxHP, TCODColor::lightRed, TCODColor::darkerRed);
	// draw the message log
	int y = 1;
	float colorCoef = 0.4f;
	for (Message **it = log.begin(); it != log.end(); it++) {
		Message *message = *it;
		con->setDefaultForeground(message->col * colorCoef);
		con->print(MSG_X, y, message->text);
		y++;
		if (colorCoef < 1.0f) {
			colorCoef += 0.3f;
		}
	}
	renderMouseLook();
	// blit the gui console on the root console
	TCODConsole::blit(con, 0, 0, engine.screenWidth, PANEL_HEIGHT, TCODConsole::root, 0, engine.screenHeight - PANEL_HEIGHT);
}
void Gui::renderBar(int x, int y, int width, const char *name, float value, float maxValue, const TCODColor &barColor, const TCODColor &backColor) {
	// fill the background
	con->setDefaultBackground(backColor);
	con->rect(x, y, width, 1, false, TCOD_BKGND_SET);
	int barWidth = (int)(value / maxValue * width);
	if (barWidth > 0) {
		// draw the bar
		con->setDefaultBackground(barColor);
		con->rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
	}
	// print text on top of the bar
	con->setDefaultForeground(TCODColor::white);
	con->printEx(x + width / 2, y, TCOD_BKGND_NONE, TCOD_CENTER, "%s : %g/%g", name, value, maxValue);
}
Gui::Message::Message(const char *text, const TCODColor &col) : text(strdup(text)), col(col) { }
Gui::Message::~Message() {
	free(text);
}
void Gui::message(const TCODColor &col, const char *text, ...) {
	// build the text
	va_list ap;
	char buf[128];
	va_start(ap, text);
	vsprintf(buf, text, ap);
	va_end(ap);
	char *lineBegin = buf;
	char *lineEnd;
	do {
		// make room for the new message
		if (log.size() == MSG_HEIGHT) {
			Message *toRemove = log.get(0);
			log.remove(toRemove);
			delete toRemove;
		}
		// detect EOL
		lineEnd = strchr(lineBegin,'\n');
		if (lineEnd) {
			*lineEnd = '\0';
		}
		// add a new message to the log
		Message *msg = new Message(lineBegin, col);
		log.push(msg);
		// go to next line
		lineBegin = lineEnd + 1;
	} while (lineEnd);
}
void Gui::renderMouseLook() {
	if (! engine.map->isInFOV(engine.mouse.cx, engine.mouse.cy)) {
		// if cursor is out of FOV, do not render
		return;
	}
	char buf[128] = " ";
	bool first = true;
	for (Actor **it = engine.actors.begin(); it != engine.actors.end(); it++) {
		Actor *actor = *it;
		// find actors under the mouse cursor
		if (actor->x == engine.mouse.cx && actor->y == engine.mouse.cy) {
			if (!first) {
				strcat(buf, ", ");
			} else {
				first = false;
			}
			strcat(buf, actor->name);
		}
	}
	// display the list of actors under the mouse cursor
	con->setDefaultForeground(TCODColor::lightGrey);
	con->print(1, 0, buf);
}
void Gui::save(TCODZip &zip) {
	zip.putInt(log.size());
	for (Message **it = log.begin(); it != log.end(); it++) {
		zip.putString((*it)->text);
		zip.putColor(&(*it)->col);
	}
}
void Gui::load(TCODZip &zip) {
	int nbMessages = zip.getInt();
	while (nbMessages > 0) {
		const char *text = zip.getString();
		TCODColor col = zip.getColor();
		message(col, text);
		nbMessages--;
	}
}

Menu::~Menu() {
	clear();
}
void Menu::clear() {
	items.clearAndDelete();
}
void Menu::addItem(MenuItemCode code, const char *label) {
	MenuItem *item=new MenuItem();
	item->code = code;
	item->label = label;
	items.push(item);
}
Menu::MenuItemCode Menu::pick() {
	static TCODImage img("menu_background1.png");
	int selectedItem = 0;
	while (!TCODConsole::isWindowClosed()) {
		img.blit2x(TCODConsole::root, 0, 0);
		int currentItem = 0;
		for (MenuItem **it = items.begin(); it != items.end(); it++) {
			if (currentItem == selectedItem) {
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			} else {
				TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
			}
			TCODConsole::root->print(10, 10 + currentItem * 3, (*it)->label);
			currentItem++;
		}
		TCODConsole::flush();
		// check key presses
		TCOD_key_t key;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
		switch (key.vk) {
			case TCODK_UP:
				selectedItem--;
				if (selectedItem < 0) {
					selectedItem = items.size() - 1;
				}
				break;
			case TCODK_DOWN:
				selectedItem = (selectedItem + 1) % items.size();
				break;
			case TCODK_ENTER:
				return items.get(selectedItem)->code;
			default:
				break;
		}
	}
	return NONE;
}
