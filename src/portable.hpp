/*FILE: portable.hpp
  Contains definitions to allow Actor objects to be picked up and carried,
  as well as definitions for specific items.
  */
class Portable {
	public:
		bool take(Actor *subject, Actor *object);
		virtual bool use(Actor *subject, Actor *object);
		void drop(Actor *subject, Actor *object);
	protected:
		enum PortableType { ANIMAL, VEGETABLE, MINERAL };
};
// definitions for specific items should go here
