class Container {
    public:
        int size; // maximum number of actors, 0 = unlimited
        TCODList<Actor *> inventory;
        
        Container(int size);
        ~Container();
        bool add(Actor *actor);
        void remove(Actor *actor);
};
