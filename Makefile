CXX = g++
PKGS = gstreamer-1.0 gstreamer-rtsp-server-1.0
CXXFLAGS = -Wall -std=c++17 `pkg-config --cflags $(PKGS)`
LDFLAGS = `pkg-config --libs $(PKGS)`
SRC = $(wildcard src/*.cpp)
OUT = ivi_player

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
