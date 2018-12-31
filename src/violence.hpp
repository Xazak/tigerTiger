/*	 violence.hpp
DATE Dec 31, 2018
AUTH xazak
DESC Definitions of Attacker class and various flavored subclasses, which handle
	combat subroutines
	Note the close interactions with Mortality, Vitality, etc
 */

class Violence {
	public:
		
	private:

};

/*
NOTES ON ATTACK PHASES
STALKING
STATS: Swiftness, Quiescence, Perception
  OBJ: Move from a far distance to within pounce range, without being detected
 FAIL: Detection by target and their subsequent escape
 		-- Target may choose to challenge instead

POUNCING
STATS: Ferocity, Swiftness, Perception
  OBJ: Ambush a target from a hiding place with swiftness and surprise
 FAIL: Target evades ambush attempt, preventing grapple initiation
 		-- Target may then attempt to escape or challenge

KILLING
STATS: Ferocity, Tenacity, Swiftness
  OBJ: Deliver a killing attack to a target while sustaining minimal damage
 FAIL: Critical HP loss, target escape

   */
