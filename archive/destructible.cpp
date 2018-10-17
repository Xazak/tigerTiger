#include <stdio.h>
#include "main.hpp"

Destructible::Destructible(float maxHP, float defense, const char *corpseName) :
	maxHP(maxHP), curHP(maxHP), defense(defense) {
		this->corpseName = strdup(corpseName);
}
Destructible::~Destructible() {
	free((char*)corpseName);
}
float Destructible::takeDamage(Actor *owner, float damage) {
	damage -= defense;
	if (damage > 0) {
		curHP -= damage;
		if (curHP <= 0) {
			die(owner);
		}
	} else {
		damage = 0;
	}
	return damage;
}
float Destructible::heal(float amount) {
	curHP += amount;
	if (curHP > maxHP) {
		amount -= curHP - maxHP;
		curHP = maxHP;
	}
	return amount;
}
void Destructible::die(Actor *owner) {
	// transform the actor into a corpse
	owner->ch = '%';
	owner->col = TCODColor::darkRed;
	owner->name = corpseName;
	owner->blocks = false;
	// make sure corpses are drawn before living
	engine.sendToBack(owner);
}
MonsterDestructible::MonsterDestructible(float maxHP, float defense, const char *corpseName) : 
	Destructible(maxHP, defense, corpseName) {
}
PlayerDestructible::PlayerDestructible(float maxHP, float defense, const char *corpseName) :
	Destructible(maxHP, defense, corpseName) {
}
void MonsterDestructible::die(Actor *owner) {
	// transform it into a corpse
	// doesn't block, can't be attacked, doesn't move
	engine.gui->message(TCODColor::lightGrey, "%s is dead.\n", owner->name);
	Destructible::die(owner);
}
void PlayerDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::red, "You died!\n", owner->name);
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}
void Destructible::load(TCODZip &zip) {
	maxHP = zip.getFloat();
	curHP = zip.getFloat();
	defense = zip.getFloat();
	corpseName = strdup(zip.getString());
}
void Destructible::save(TCODZip &zip) {
	zip.putFloat(maxHP);
	zip.putFloat(curHP);
	zip.putFloat(defense);
	zip.putString(corpseName);
}
void PlayerDestructible::save(TCODZip &zip) {
	zip.putInt(PLAYER);
	Destructible::save(zip);
}
void MonsterDestructible::save(TCODZip &zip) {
	zip.putInt(MONSTER);
	Destructible::save(zip);
}
Destructible *Destructible::create(TCODZip &zip) {
	DestructibleType type = (DestructibleType)zip.getInt();
	Destructible *destructible = NULL;
	switch(type) {
		case MONSTER: destructible = new MonsterDestructible(0, 0, NULL); break;
		case PLAYER: destructible = new PlayerDestructible(0, 0, NULL); break;
	}
	destructible->load(zip);
	return destructible;
}
