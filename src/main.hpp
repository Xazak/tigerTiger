/* main.hpp - Oct 18 2018
 * This file is a 'master' header; it will contain all of the individual
 * headers for the entire project so that cpp files need only invoke this file.
 */

#include "libtcod.hpp"
class Actor; // this is a 'forward declaration'
// other implementation files go here
#include "sentience.hpp"
#include "vitality.hpp"
#include "mortality.hpp"
#include "chrono.hpp"
#include "portable.hpp"
#include "container.hpp"
#include "actor.hpp"
#include "gui.hpp"
#include "map.hpp"
#include "engine.hpp"
#include <iostream>
#include <assert.h>

// *** DEBUG TOOLS AND DEFINITIONS ***
// Enable debugging macros; comment out to remove
#define DEBUG

// Defines a debug macro for sending information to cerr easily. The contents
// of x will be rendered as literally as possible. Include quotes and newlines.
//		xxxMSG( "Debug Message\n" )
// The do-while statement ensures that x is interpreted correctly.
#ifdef DEBUG
#define LOGMSG(x) do { std::clog << __FILE__ << ":" << __LINE__ << ":" << __func__ << "() - " << x << std::endl; } while (0)
#else
#define LOGMSG(x)
#endif

#ifdef DEBUG
#define ERRMSG(x) do { std::cerr << __FILE__ << ":" << __LINE__ << ":" << __func__ << "() - " << x << std::endl; } while (0)
#else
#define ERRMSG(x)
#endif
