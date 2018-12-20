/*	 container.hpp
DATE Oct 13 2018
AUTH xazak
DESC Contains definitions for containers of Actor objects.
 */
class Container {
	public:
		Container(int size);
		Container(TCODZip &fileBuffer);
		~Container();
		bool add(Actor *object);
		void remove(Actor *object);
		void save(TCODZip &fileBuffer);
		void load(TCODZip &fileBuffer);
		int size; // maximum quantity of stored objects, where 0 = unlimited
		TCODList<Actor *> inventory; // pointer to TCODList of Actor *
};
