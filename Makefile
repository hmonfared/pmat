CXXFLAGS =	-O0 -g -Wall -fmessage-length=0 -std=c++0x

OBJS =		testmain.o
CXX=g++47
LIBS =

TARGET =	pmat

$(TARGET):	$(OBJS)
	$(CXX)  -o $(TARGET)  $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
