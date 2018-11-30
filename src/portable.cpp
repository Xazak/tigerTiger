/*	 portable.cpp
DATE Oct 25 2018
AUTH xazak
DESC Implements fxns to allow Actor objects to be picked up and carried,
	 as well as behavior for specific items.
 */
#include "main.hpp"

bool Portable::take (Actor *subject, Actor *object) {
	// can the subject contain the object AND (implicit) was adding the object
	// to the subject's inventory successful?
	if (subject->container && subject->container->add(object)) {
		engine.allActors.remove(object); // remove it from the global actor list
		return true;
	}
	return false;
}
bool Portable::use (Actor *subject, Actor *object) {
	// this assumes all items are single-use; is there a way to prevent this?
	if (subject->container) {
		subject->container->remove(object);
		delete object;
		return true;
	}
	return false;
}
void Portable::drop (Actor *subject, Actor *object) {
	if (subject->container) {
		subject->container->remove(object);
		engine.allActors.push(object);
		object->xpos = subject->xpos;
		object->ypos = subject->ypos;
		engine.gui->message(TCODColor::lightGrey, "%s drops a %s.\n", subject->name, object->name);
	}
}
