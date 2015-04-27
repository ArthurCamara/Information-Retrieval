
PROGNAME := tp1ri

CPPFLAGS := -DUSE_BOOST

CFLAGS := -Wall -O3

CXXFLAGS += -Wall -O3 -std=c++11

LDFLAGS  += -g `pkg-config --cflags --libs gumbo` -L/opt/lib -lz

CXX := g++

BOOST_MODULES = \
  date_time     \
  filesystem    \
  graph         \
  iostreams     \
  math_c99      \
  system        \
  serialization \
  regex

BOOST_MODULES_TYPE := -mt

BOOST_MODULES_LIBS := $(addsuffix $(BOOT_MODULES_TYPE),$(BOOST_MODULES))

BOOST_LDFLAGS := $(addprefix -lboost_,$(BOOST_MODULES_LIBS))

CPPFLAGS += $(BOOST_CPPFLAGS)
LDFLAGS += $(BOOST_LDFLAGS)

SOURCES = $(wildcard *.cpp) $(wildcard src/indexer/*.cpp) $(wildcard src/QueryProcessing/*.cpp) $(wildcard lib/RiCode/*.cpp)

HEADERS = $(wildcard %.h)

OBJECTS = $(SOURCES:%.cpp=%.o)

all : $(PROGNAME)

$(PROGNAME) : $(OBJECTS) Makefile
	$(CXX) -o $@ $(LDFLAGS) $(OBJECTS)

%.o: %.cpp $(HEADERS) Makefile
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $(OUTPUT_OPTION) $<
				
clean :
	@echo "Clean."
	-rm -f *.o $(PROGNAME)