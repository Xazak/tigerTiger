/*	 container.cpp
DATE Oct 13 2018
AUTH xazak
DESC Implementation of containers of Actor objects.
 */
#include "main.hpp"

Container::Container (int size): size(size) { }
Container::Container (TCODZip &fileBuffer) {
	LOGMSG("called");
	load(fileBuffer);
}
Container::~Container() {
	inventory.clearAndDelete();
}
bool Container::add (Actor *object) {
	// is the actor's inventory full?
	if (size > 0 && inventory.size() >= size) return false;
	inventory.push(object); // add the object to the actor's inventory stack
	return true;
}
void Container::remove (Actor *object) {
	inventory.remove(object);
}
void Container::save(TCODZip &fileBuffer) {
	LOGMSG("called");
	fileBuffer.putInt(size); // maximum inventory capacity
	fileBuffer.putInt(inventory.size()); // actual inventory size
	for (Actor **iter = inventory.begin(); iter != inventory.end(); iter++) {
		(*iter)->save(fileBuffer);
	}
}
void Container::load(TCODZip &fileBuffer) {
	LOGMSG("called");
	size = fileBuffer.getInt();
	int contents = fileBuffer.getInt();
	while (contents > 0) {
		Actor *newItem = new Actor(0, 0, 0, TCODColor::white, NULL);
		newItem->load(fileBuffer);
		inventory.push(newItem);
		contents--;
	}
}
