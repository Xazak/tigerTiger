/*	 saveload.hpp
DATE Dec 17 2018
AUTH xazak
DESC Defines the FileMachine and FilePiece classes for external storage purposes
 */

// Defines the basic interface object: other classes will instantiate from this
class FileInterface {
	public:
//		virtual ~FileMachine() {}
		virtual void save() = 0;
		virtual void load() = 0;
};

// Defines the controlling engine, which will handle the file interfaces
class FileEngine: public FileInterface {
	public:
		virtual void save();
		virtual void load();

};
