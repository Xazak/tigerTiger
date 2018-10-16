#include "main.hpp"
/*
void Engine::save() {
	if (player->destructible->isDead()) {
		TCODSystem::deleteFile("game.sav");
	} else {
		TCODZip zip;
		//save the map first
		zip.putInt(map->width);
		zip.putInt(map->height);
		map->save(zip);
		// then the player
		player->save(zip);
		// then the other actors
		zip.putInt(actors.size()-1);
		for (Actor **it = actors.begin(); it != actors.end(); it++) {
			if (*it != player) {
				(*it)->save(zip);
			}
		}
		// then the message log
		gui->save(zip);
		zip.saveToFile("game.sav");
	}
}
void Engine::load() {
	if (TCODSystem::fileExists("game.sav")) {
		TCODZip zip;
		zip.loadFromFile("game.sav");
		// load the map
		int width = zip.getInt();
		int height = zip.getInt();
		map = new Map(width, height);
		map->load(zip);
		// load the player
		player = new Actor(0, 0, 0, NULL, TCODColor::white);
		player->load(zip);
		actors.push(player);
		// load everyone else
		int nbActors = zip.getInt();
		while (nbActors > 0) {
			Actor *actor = new Actor(0, 0, 0, NULL, TCODColor::white);
			actor->load(zip);
			actors.push(actor);
			nbActors--;
		}
		// load the message log
		gui->load(zip);
	} else {
		engine.init();
	}
}
*/
