# VARIABLES
NAME=tigerGame

INCDIR=include/libtcod
SRCDIR=src
OBJDIR=src

SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:.cpp=.o)

CXXFLAGS=-Wall -g -I$(INCDIR) -std=c++14
# check if on linux or windows and set the linker flags accordingly
ifeq ($(shell sh -c 'uname -s'),Linux)
#  LIBS=-L. -ltcod_debug -ltcodgui_debug -Wl,-rpath=.
  LIBS=-L. -ltcod -ltcodgui -Wl,-rpath=.
#else
#  LIBS=-Llib -ltcod-mingw-debug -static-libgcc -static-libstdc++
endif
LDFLAGS=$(LIBS)

tigerGame:	$(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
	$(CXX) $< -c -o $@ $(CXXFLAGS)

#GUARANTEED WORKING COMPILE LINE
basic:
	g++ src/*.cpp -o tigerGame -Iinclude/libtcod -L. -ltcod -ltcodgui -Wl,-rpath=. -Wall

deprecated:	$(OBJS)
	$(CXX) -E -dD $@ $^ -Wall -I$(INCDIR) $(LDFLAGS)

.PHONY:	clean

clean:	#clean up working dir
	rm -rf $(OBJDIR)/*.o $(NAME)
