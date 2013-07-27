CXXFLAGS =	-O0 -g -Wall -fmessage-length=0 -std=c++11 -fPIC

OBJS =		matrix.o

LIBS =

TARGET =	pmat.so

$(TARGET):	$(OBJS)
	$(CXX)  -o $(TARGET) -shared $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
