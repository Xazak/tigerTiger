/*	 container.hpp
DATE Oct 13 2018
AUTH xazak
DESC Contains definitions for containers of Actor objects.
 */
class Container {
	public:
		int size; // maximum quantity of stored objects, where 0 = unlimited
		TCODList<Actor *> inventory; // pointer to TCODList of Actor *
		Container(int size);
		~Container();
		bool add(Actor *object);
		void remove(Actor *object);
		//load and save fxns
};
