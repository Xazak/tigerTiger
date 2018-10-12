#include <stdio.h>
#include "main.hpp"

Destructible::Destructible(float maxHP, float defense, const char *corpseName) :
    maxHP(maxHP), curHP(maxHP), defense(defense), corpseName(corpseName) {
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
