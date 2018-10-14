#include "main.hpp"

bool Pickable::pick(Actor *owner, Actor *wearer) {
    if (wearer->container && wearer->container->add(owner)) {
        engine.actors.remove(owner);
        return true;
    }
    return false;
}
bool Pickable::use(Actor *owner, Actor *wearer) {
    if (wearer->container) {
        wearer->container->remove(owner);
        delete owner;
        return true;
    }
    return false;
}
void Pickable::drop(Actor *owner, Actor *wearer) {
    if (wearer->container) {
        wearer->container->remove(owner);
        engine.actors.push(owner);
        owner->x = wearer->x;
        owner->y = wearer->y;
        engine.gui->message(TCODColor::lightGrey, "%s drops a %s.", wearer->name, owner->name);
    }
}
Healer::Healer(float amount) : amount(amount) { }
bool Healer::use(Actor *owner, Actor *wearer) {
    if (wearer->destructible) {
        float amountHealed = wearer->destructible->heal(amount);
        if (amountHealed > 0) {
            return Pickable::use(owner, wearer);
        }
    }
    return false;
}
LightningBolt::LightningBolt(float range, float damage) : range(range), damage(damage) { }
bool LightningBolt::use(Actor *owner, Actor *wearer) {
    Actor *closestMonster = engine.getClosestMonster(wearer->x, wearer->y, range);
    if (!closestMonster) {
        engine.gui->message(TCODColor::lightGrey, "No enemies in range.");
        return false;
    }
    // hit closest monster for <damage> HP
    engine.gui->message(TCODColor::lightBlue,
            "A lightning bolt strikes the %s with a crash!\n"
            "It deals %g damage.", closestMonster->name, damage);
    closestMonster->destructible->takeDamage(closestMonster, damage);
    return Pickable::use(owner, wearer);
}
Fireball::Fireball(float range, float damage) : LightningBolt(range, damage) { }
bool Fireball::use(Actor *owner, Actor *wearer) {
    engine.gui->message(TCODColor::cyan, "Left-click a target tile for the fireball,\nor right-click to exit.");
    int x, y;
    if (!engine.pickATile(&x, &y)) {
        return false;
    }
    // burn everything in range, including player
    engine.gui->message(TCODColor::orange, "The fireball explodes, burning everything within %g tiles!", range);
    for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
        Actor *actor = *iterator;
        if (actor->destructible && !actor->destructible->isDead() && actor->getDistance(x, y) <= range) {
            engine.gui->message(TCODColor::orange, "The %s gets burned for %g damage.", actor->name, damage);
            actor->destructible->takeDamage(actor, damage);
        }
    }
    return Pickable::use(owner, wearer);
}
Confuser::Confuser(int nbTurns, float range) : nbTurns(nbTurns), range(range) { }
bool Confuser::use(Actor *owner, Actor *wearer) {
    engine.gui->message(TCODColor::cyan, "Left-click an enemy to confuse it,\nor right-click to cancel.");
    int x, y;
    if (!engine.pickATile(&x, &y, range)) {
        return false;
    }
    Actor *actor = engine.getActor(x, y);
    if (!actor) return false;
    // confuse the monster for <nbTurns> turns
    Ai *confusedAi = new ConfusedMonsterAi(nbTurns, actor->ai);
    actor->ai = confusedAi;
    engine.gui->message(TCODColor::lightGreen, "The eyes of the %s look vacant\n as they stumble around.", actor->name);
    return Pickable::use(owner, wearer);
}
