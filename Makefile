CXX = g++
CXXFLAGS = -Wall -std=c++17 `pkg-config --cflags gstreamer-1.0`
LDFLAGS = `pkg-config --libs gstreamer-1.0`
SRC = src/main.cpp
OUT = ivi_player

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)


