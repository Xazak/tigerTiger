/*	 container.cpp
DATE Oct 13 2018
AUTH xazak
DESC Implementation of containers of Actor objects.
 */
#include "main.hpp"

Container::Container (int size): size(size) { }
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
